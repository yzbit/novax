/************************************************************************************
The MIT License

Copyright (c) 2024 YaoZinan  [zinan@outlook.com, nvx-quant.com]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

* \author: yaozn(zinan@outlook.com)
* \date: 2024
**********************************************************************************/

#include "tradeproxy.h"

#include "../log.hpp"
#include "comm.h"

NVX_NS_BEGIN

#define CTP_CHECK_READY( _r_ )              \
    do {                                    \
        if ( !_api ) {                      \
            LOG_INFO( "ctp not initiaed" ); \
            return _r_;                     \
        }                                   \
        if ( !settled() ) {                 \
            LOG_INFO( "ctp not settled" );  \
            return _r_;                     \
        }                                   \
    } while ( 0 )

namespace ctp {

trader::trader( ipub* p_, int id_start_ref_ )
    : broker( p_ )
    , _last_ref{ id_start_ref_ } {}

nvx_st trader::start() {
    if ( _settings.load( CTP_TRADE_SETTING_FILE ) < 0 ) {
        LOG_TAGGED( "ctp", "load config failed: %s", CTP_TRADE_SETTING_FILE );
        return NVX_FAIL;
    }

    LOG_INFO( "RegisterSpi@注册ctp交易网关" );
    std::string flow = _settings.flow_path;

    LOG_INFO( "RegisterSpi@创建交易网关数据流目录：%s", flow.c_str() );
    LOG_INFO( "RegisterSpi@[ctp]创建交易网关数据流目录：%s", flow.c_str() );
    _api = CThostFtdcTraderApi::CreateFtdcTraderApi( flow.c_str() );

    LOG_INFO( "RegisterSpi@{ctp}初始化交易网关:注册事件" );

    _api->RegisterSpi( this );                         // 注册事件类
    _api->SubscribePublicTopic( THOST_TERT_RESTART );  // 订阅公共流
    _api->SubscribePrivateTopic( THOST_TERT_QUICK );   // 订阅私有流

    LOG_INFO( "RegisterSpi@{ctp}初始化交易网关:注册交易前端" );

    for ( auto& a : _settings.frontend ) {
        // char *s = "tcp://180.168.146.187:10010";
        // todo 设置多个地址和一次性还是循环调用
        _api->RegisterFront( const_cast<char*>( a.c_str() ) );  // 设置交易前置地址,设置多个，内部ctp可以自动切换
        LOG_INFO( "RegisterSpi@[ctp] tfront=%s", a.c_str() );
    }

    LOG_INFO( "{ctp} init" );
    _api->Init();

    //_api.join

    return NVX_OK;
}

nvx_st trader::stop() {
    LOG_INFO( "ReleaseSpi@释放ctp交易网关" );

    // ctp 手册建议:
    _api->RegisterSpi( nullptr );
    _api->Release();

    return NVX_OK;
}

nvx_st trader::qry_fund() {
    CTP_CHECK_READY( NVX_FAIL );

    CThostFtdcQryTradingAccountField acct = {};
    strncpy( acct.BrokerID, _settings.i.broker.c_str(), sizeof( acct.BrokerID ) );
    strncpy( acct.InvestorID, _settings.i.id.c_str(), sizeof( acct.InvestorID ) );
    strncpy( acct.CurrencyID, "CNY", sizeof( acct.CurrencyID ) );

    auto rc = _api->ReqQryTradingAccount( &acct, req_id() );
    if ( 0 == rc ) {
        LOG_INFO( "funds qry sent" );
        return NVX_OK;
    }

    LOG_INFO( "funds qry fail=%d", rc );
    return NVX_FAIL;
}

nvx_st trader::qry_marginrate() {
    return NVX_OK;
}

nvx_st trader::qry_commission() {
    CTP_CHECK_READY( NVX_FAIL );

    CThostFtdcQryInstrumentCommissionRateField cmmsn = {};

    strncpy( cmmsn.BrokerID, _settings.i.broker.c_str(), sizeof( cmmsn.BrokerID ) );
    strncpy( cmmsn.InvestorID, _settings.i.id.c_str(), sizeof( cmmsn.InvestorID ) );
    // todo
    //  strncpy( filed.InstrumentID, _settings..c_str() );
    auto rc = _api->ReqQryInstrumentCommissionRate( &cmmsn, req_id() );

    if ( 0 == rc ) {
        LOG_INFO( "commistion query req sent" );
        return NVX_OK;
    }
    else {
        LOG_INFO( "commistion query req fail=%d", rc );
    }

    return NVX_FAIL;
}

void trader::reconnected( const session& s_, const ordref& max_ref_ ) {
    LOG_INFO( "recoonected: font=%d, conn=%d", s_.front, s_.conn );

    _ss = s_;

    if ( _last_ref < max_ref_ ) {
        LOG_INFO( "reset start id [%s] to [%s]", _last_ref.data(), max_ref_.data() );
        _last_ref = max_ref_;
    }

    // if ( !_settled ) {
    qry_settlement();
    //}
}

nvx_st trader::qry_position() {
    return NVX_OK;
}

nvx_st trader::cancel( const oid& id_ ) {
#if 0
    CTP_CHECK_READY( NVX_FAIL );

    CThostFtdcInputOrderActionField r = {};

    // UserID非必填，是操作员账号，不填写会收不到OnErrRtnOrderAction回报，一般客户和InvestorID填写一样就可以
    strncpy( r.BrokerID, _settings.i.broker.c_str(), sizeof( r.BrokerID ) );
    strncpy( r.InvestorID, _settings.i.id.c_str(), sizeof( r.InvestorID ) );
    strncpy( r.UserID, _settings.i.id.c_str(), sizeof( r.UserID ) );

    auto id = _id.id_of( id_ );

    if ( !id ) {
        LOG_INFO( "xModifyOrder@试图删除一个不存在订单ID,%u", id_ );
        return NVX_FAIL;
    }

    if ( id->has_sysid() ) {
        LOG_INFO( "delete order by syseid ex=%s oid=%s", id.value().ex.data(), id.value().id.data() );
        id.value().ex.copy_to( r.ExchangeID );
        id.value().id.copy_to( r.OrderSysID );
    }
    else {
        LOG_INFO( "delete order by ref: %s", id.value().fsr.ref.data );
        id.value().fsr.ref.copy_to( r.OrderRef );
        r.FrontID   = _ss.front;
        r.SessionID = _ss.conn;
    }

    strncpy( r.InstrumentID, o_.code.c_str(), sizeof( r.InstrumentID ) );

    r.ActionFlag = THOST_FTDC_AF_Delete;

    LOG_INFO( "-------------------------------ctp撤单参数----------------------------------\n" );
    LOG_INFO( "BrokerID = %s, InvestorID=%s\
        ExchangeID=%s,OrderSysID=%s\
        FrontID=%d,SessionID=%d,OrderRef=%s\
        ActionFlag=%d,InstrumentID=%s\n",
              r.BrokerID,
              r.InvestorID,
              r.ExchangeID,
              r.OrderSysID,
              r.FrontID,
              r.SessionID,
              r.OrderRef,
              r.ActionFlag,
              r.InstrumentID );

    auto rc = _api->ReqOrderAction( &r, req_id() );
    if ( 0 == rc ) {
        LOG_INFO( "delete req send ok" );
        return NVX_OK;
    }

    LOG_INFO( "delete req fail=%d", rc );
#endif
    return NVX_FAIL;
}

oid trader::put( const code& instrument_, vol qty_, price price_, ord_type mode_, ord_dir dir_ ) {
    LOG_INFO( "xPutOrder@ctp准备下单" );
    CTP_CHECK_READY( NVX_BAD_OID );

    CThostFtdcInputOrderField r = {};

    ordref ref;  // todo = ++_last_ref;
    LOG_INFO( "new order id=%s, %u", ref.data(), ref.int_val() );

    ref.copy_to( r.OrderRef );

    strncpy( r.BrokerID, _settings.i.broker.c_str(), sizeof( r.BrokerID ) );
    strncpy( r.InvestorID, _settings.i.id.c_str(), sizeof( r.InvestorID ) );
    strncpy( r.UserID, _settings.i.id.c_str(), sizeof( r.UserID ) );
    strncpy( r.InstrumentID, instrument_.c_str(), sizeof( r.InstrumentID ) );

    r.IsSwapOrder         = 0;
    r.ForceCloseReason    = THOST_FTDC_FCC_NotForceClose;
    r.MinVolume           = 0;
    r.ContingentCondition = THOST_FTDC_CC_Immediately;
    r.VolumeTotalOriginal = ( TThostFtdcVolumeType )qty_;
    r.LimitPrice          = price_;
    r.CombHedgeFlag[ 0 ]  = THOST_FTDC_HF_Speculation;
    r.ForceCloseReason    = THOST_FTDC_FCC_NotForceClose;
    r.IsAutoSuspend       = 0;
    r.UserForceClose      = 0;

    LOG_INFO( "order mode:%d", mode_ );

    switch ( mode_ ) {
    default:
        LOG_INFO( "不存在的订单模式:%d", mode_ );
        return NVX_FAIL;

    case ord_type::market:
        r.OrderPriceType  = THOST_FTDC_OPT_AnyPrice;
        r.TimeCondition   = THOST_FTDC_TC_IOC;
        r.VolumeCondition = THOST_FTDC_VC_AV;
        r.LimitPrice      = 0;
        break;

    case ord_type::fak:
        r.OrderPriceType  = THOST_FTDC_OPT_LimitPrice;
        r.TimeCondition   = THOST_FTDC_TC_IOC;
        r.VolumeCondition = THOST_FTDC_VC_MV;
        r.MinVolume       = 1;
        break;

    case ord_type::fok:
        r.OrderPriceType  = THOST_FTDC_OPT_LimitPrice;
        r.TimeCondition   = THOST_FTDC_TC_IOC;
        r.VolumeCondition = THOST_FTDC_VC_CV;
        break;

    case ord_type::wok:
        r.OrderPriceType  = THOST_FTDC_OPT_LimitPrice;
        r.TimeCondition   = THOST_FTDC_TC_GFD;  //! 当日有效
        r.VolumeCondition = THOST_FTDC_VC_AV;
        break;
    }

    LOG_INFO( "order direction: %d", dir_ );

    switch ( dir_ ) {
    default:
        LOG_INFO( "bad order direction: %d", dir_ );
        return NVX_FAIL;

    case ord_dir::p_long:
        r.Direction           = THOST_FTDC_D_Buy;
        r.CombOffsetFlag[ 0 ] = THOST_FTDC_OF_Open;
        break;

    case ord_dir::sell:
        r.Direction           = THOST_FTDC_D_Sell;
        r.CombOffsetFlag[ 0 ] = THOST_FTDC_OF_Close;
        break;

    case ord_dir::p_short:
        r.Direction           = THOST_FTDC_D_Sell;
        r.CombOffsetFlag[ 0 ] = THOST_FTDC_OF_Open;
        break;

    case ord_dir::cover:
        r.Direction           = THOST_FTDC_D_Buy;
        r.CombOffsetFlag[ 0 ] = THOST_FTDC_OF_Close;
        break;
    }

    LOG_INFO( "---------------------------ctp下单参数------------------------------------------\n" );
    LOG_INFO( "\nbroker=%s\ninvestor=%s\ncode=%s\nref=%s\n\
    LimitPrice=%lf\nVolumeTotalOriginal=%d\nContingentCondition=%d\n\
    MinVolume=%d\nOrderPriceType=%d\nTimeCondition=%d\n\
    VolumeCondition=%d\nDirection=%d\n\
    CombOffsetFlag[0]=%d\nCombHedgeFlag=%d\n\
    ForceCloseReason=%d\nIsAutoSuspend=%d\nUserForceClose=%d\n",
              r.BrokerID,
              r.InvestorID,
              r.InstrumentID,
              r.OrderRef,
              r.LimitPrice,
              r.VolumeTotalOriginal,
              r.ContingentCondition,
              r.MinVolume,
              r.OrderPriceType,
              r.TimeCondition,
              r.VolumeCondition,
              r.Direction,
              r.CombOffsetFlag,
              r.CombHedgeFlag,
              r.ForceCloseReason,
              r.IsAutoSuspend,
              r.UserForceClose );

    r.RequestID = req_id();

    auto rc = _api->ReqOrderInsert( &r, r.RequestID );

    if ( 0 == rc ) {
        LOG_INFO( "req id=%d", r.RequestID );
        _id.insert( { { _ss.front, _ss.conn, ref }, {}, {} } );
        return ( oid )ref.int_val();
    }

    LOG_INFO( "Insert order Fail=%d", rc );
    return NVX_BAD_OID;
}

nvx_st trader::login() {
    LOG_INFO( "xLogin@登录交易网关:%s %s", _settings.i.broker.c_str(), _settings.i.name.c_str() );

    CTP_CHECK_READY( NVX_FAIL );
    CThostFtdcReqUserLoginField ulog = {};

    strncpy( ulog.BrokerID, _settings.i.broker.c_str(), sizeof( ulog.BrokerID ) );
    strncpy( ulog.UserID, _settings.i.name.c_str(), sizeof( ulog.UserID ) );
    strncpy( ulog.Password, _settings.i.password.c_str(), sizeof( ulog.Password ) );

    auto rc = _api->ReqUserLogin( &ulog, req_id() );

    if ( 0 == rc ) {
        LOG_INFO( "req login success" );
        return NVX_OK;
    }

    LOG_INFO( "req login fail" );

    return NVX_FAIL;
}

nvx_st trader::logout() {
    CTP_CHECK_READY( NVX_FAIL );

    CThostFtdcUserLogoutField ulog = {};

    strncpy( ulog.BrokerID, _settings.i.broker.c_str(), sizeof( ulog.BrokerID ) );
    strncpy( ulog.UserID, _settings.i.name.c_str(), sizeof( ulog.UserID ) );

    auto rc = _api->ReqUserLogout( &ulog, req_id() );
    if ( 0 == rc ) {
        LOG_INFO( "logout success" );
        return NVX_OK;
    }

    LOG_INFO( "req login fail:broker=%s, user=%s", ulog.BrokerID, ulog.UserID );
    return NVX_FAIL;
}

void trader::OnRtnBulletin( CThostFtdcBulletinField* b_ ) {
    LOG_INFO( "\n#notification,ex=%s,day=%s,id=%d,no=%d,type=%s,urgent=%d,time=%s,abs=%s,from=%s,content=%s,link=%s,market=%s\n",
              b_->ExchangeID,
              b_->TradingDay,
              b_->BulletinID,
              b_->SequenceNo,
              b_->NewsType,
              b_->NewsUrgency,
              b_->SendTime,
              b_->Abstract,
              b_->ComeFrom,
              b_->Content,
              b_->URLLink,
              b_->MarketID );
}

void trader::OnRtnTradingNotice( CThostFtdcTradingNoticeInfoField* f_ ) {
    LOG_INFO( "\n#message, series=%d,no=%d, unit=%s,broker=%s, investor=%s,time=%s, msg=[%s]\n",
              f_->SequenceSeries,
              f_->SequenceNo,
              f_->InvestUnitID,
              f_->BrokerID,
              f_->InvestorID,
              f_->SendTime,
              f_->FieldContent );
}

nvx_st trader::auth() {
    LOG_INFO( "xReqAuth@auth client:%s %s %s", _settings.c.auth.c_str(), _settings.c.appid.c_str(), _settings.c.token.c_str() );

    CTP_CHECK_READY( NVX_FAIL );
    CThostFtdcReqAuthenticateField a = {};

    strncpy( a.BrokerID, _settings.i.broker.c_str(), sizeof( a.BrokerID ) );
    strncpy( a.UserID, _settings.i.name.c_str(), sizeof( a.UserID ) );
    strncpy( a.AppID, _settings.c.appid.c_str(), sizeof( a.AppID ) );
    strncpy( a.AuthCode, _settings.c.token.c_str(), sizeof( a.AuthCode ) );

    auto rc = _api->ReqAuthenticate( &a, req_id() );
    if ( 0 == rc ) {
        LOG_INFO( "authoration req success" );
        return NVX_OK;
    }

    LOG_INFO( "authoration req fail=%d", rc );
    return NVX_FAIL;
}

void trader::OnFrontConnected() {
    LOG_INFO( ">> 交易网关链接成功,send auth req" );
    auth();
}

nvx_st trader::qry_settlement() {
    CTP_CHECK_READY( NVX_FAIL );

    CThostFtdcQrySettlementInfoField settle = {};

    strncpy( settle.BrokerID, _settings.i.broker.c_str(), sizeof( settle.BrokerID ) );
    strncpy( settle.InvestorID, _settings.i.id.c_str(), sizeof( settle.InvestorID ) );

    LOG_INFO( "query settlement sheet:broker=%s, investor=%s", settle.BrokerID, settle.InvestorID );

    /*! 不能带有日期参数，否则会失败*/
    // strcpy(settleInfoReq.TradingDay, m_tradingDay);

    auto req = req_id();
    auto rc  = CTP_SYNC.wait( req, 2000, _api, &CThostFtdcTraderApi::ReqQrySettlementInfo, &settle, req );

    if ( rc.size() > 0 ) {
        return confirm_settlement();
    }

    return NVX_FAIL;
}

nvx_st trader::confirm_settlement() {
    CTP_CHECK_READY( NVX_FAIL );

    CThostFtdcSettlementInfoConfirmField confirm = {};

    strncpy( confirm.BrokerID, _settings.i.broker.c_str(), sizeof( confirm.BrokerID ) );
    strncpy( confirm.InvestorID, _settings.i.id.c_str(), sizeof( confirm.InvestorID ) );

    auto rc = _api->ReqSettlementInfoConfirm( &confirm, req_id() );
    if ( 0 == rc ) {
        LOG_INFO( "confirm settlement success" );
        _settled = true;
        return NVX_OK;
    }

    LOG_INFO( "confirm settlement fail=%d", rc );

    return NVX_FAIL;
}

void trader::OnRspAuthenticate( CThostFtdcRspAuthenticateField* pRspAuthenticateField, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    LOG_INFO( "authoration response" );

    if ( pRspInfo != NULL && pRspInfo->ErrorID == 0 ) {
        LOG_INFO( "认证成功,ErrorID=%d, ErrMsg=%s", pRspInfo->ErrorID, pRspInfo->ErrorMsg );
        login();
    }
    else {
        LOG_INFO( "认证失败,ErrorID=%d, ErrMsg=%s", pRspInfo->ErrorID, pRspInfo->ErrorMsg );
    }
}

void trader::OnRspUserLogin( CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    // todo
    // reconnected( { pRspUserLogin->FrontID, pRspUserLogin->SessionID }, { pRspUserLogin->MaxOrderRef } );
}

void trader::OnRspError( CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    LOG_INFO( "rsp error: id=%d msg=%s", pRspInfo->ErrorID, pRspInfo->ErrorMsg );
}

void trader::OnFrontDisconnected( int nReason ) {
    LOG_INFO( "front disconnted:reason=%d", nReason );
}

void trader::OnHeartBeatWarning( int nTimeLapse ) {
}

void trader::OnRspUserLogout( CThostFtdcUserLogoutField* pUserLogout, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    LOG_ERROR_AND_RET( pRspInfo, nRequestID, bIsLast );

    LOG_INFO( "user logout %s", pUserLogout->UserID );
}

void trader::OnRspSettlementInfoConfirm( CThostFtdcSettlementInfoConfirmField* f, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    LOG_INFO( "投资者结算结果确认成功,确认日期：%s %s", f->ConfirmDate, f->ConfirmTime );
}

void trader::OnRspQrySettlementInfo( CThostFtdcSettlementInfoField* pSettlementInfo, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    LOG_INFO( "settlement received, req=%d last=%d", nRequestID, bIsLast );
    CTP_SYNC.update( nRequestID, pSettlementInfo, sizeof( CThostFtdcSettlementInfoField ), bIsLast );
}

void trader::OnRspOrderInsert( CThostFtdcInputOrderField* f_, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    LOG_INFO( "订单被ctp前置拒绝,req=%d, id=%d, msg=%s", nRequestID, pRspInfo->ErrorID, pRspInfo->ErrorMsg );
    auto id = _id.id_of( { _ss.front, _ss.conn, ordref( f_->OrderRef ) } );

    if ( !id ) {
        LOG_INFO( "can not reg order ref: %s", f_->OrderRef );
        return;
    }

    // todo
    // delegator()->update_ord( id->fsr.ref.int_val(), ord_status::cancelled );
}

void trader::OnRspOrderAction( CThostFtdcInputOrderActionField* f_, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    LOG_INFO( "撤单被ctp前置拒绝,req=%d, id=%d, msg=%s", nRequestID, pRspInfo->ErrorID, pRspInfo->ErrorMsg );

    auto id = _id.id_of( { _ss.front, _ss.conn, ordref( f_->OrderRef ) } );

    if ( !id ) {
        LOG_INFO( "rtn trade, cannot reg order id" );
        return;
    }

    // todo
    //  delegator()->update_ord( id->fsr.ref.int_val(), ord_status::aborted );
}

ord_dir trader::cvt_direction( const TThostFtdcDirectionType& di_, const TThostFtdcCombOffsetFlagType& comb_ ) {
    if ( di_ == THOST_FTDC_D_Buy && comb_[ 0 ] == '0' )
        return ord_dir::p_long;
    else if ( di_ == THOST_FTDC_D_Buy && comb_[ 0 ] == '1' )
        return ord_dir::cover;
    else if ( di_ == THOST_FTDC_D_Sell && comb_[ 0 ] == '0' )
        return ord_dir::p_short;
    else if ( di_ == THOST_FTDC_D_Sell && comb_[ 0 ] == '1' )
        return ord_dir::sell;
    else
        return ord_dir::none;
}

ord_dir trader::cvt_direction( const TThostFtdcDirectionType& di_, const TThostFtdcOffsetFlagType& comb_ ) {
    bool open = comb_ == THOST_FTDC_OF_Open;

    if ( di_ == THOST_FTDC_D_Buy && open )
        return ord_dir::p_long;
    else if ( di_ == THOST_FTDC_D_Buy && !open )
        return ord_dir::cover;
    else if ( di_ == THOST_FTDC_D_Sell && open )
        return ord_dir::p_short;
    else if ( di_ == THOST_FTDC_D_Sell && !open )
        return ord_dir::sell;
    else
        return ord_dir::none;
}

void trader::OnRtnOrder( CThostFtdcOrderField* f_ ) {
    LOG_INFO( "[ctp] OnOrderRtn[1]@front=%d sess=%d ref=%s exid=%s sysid=%s", f_->FrontID, f_->SessionID, f_->OrderRef, f_->ExchangeID, f_->OrderSysID );

    auto id = f_->RelativeOrderSysID[ 0 ] != 0
                  ? _id.id_of( f_->ExchangeID, f_->RelativeOrderSysID )
                  : _id.id_of( { f_->FrontID, f_->SessionID, ordref( f_->OrderRef ) } );

    if ( !id ) {
        LOG_INFO( "cannot reg order id ex= %s syso=%d ref=%s rel=%s", f_->ExchangeID, f_->OrderSysID, f_->OrderRef, f_->RelativeOrderSysID );
        return;
    }

    oid oid = id->fsr.ref.int_val();
    LOG_INFO( "act id=%u", oid );

    LOG_INFO( "RTN status: code=%s {front=%d sess=%d ref=%s} \n{exid=%s sysid=%s}, \n{sum status=%d, status=%d ,day=%s time=%s}, \n{limitprice=%lf voltraded=%d vaolremain=%d}",
              f_->InstrumentID,
              f_->FrontID,
              f_->SessionID,
              f_->OrderRef,
              f_->ExchangeID,
              f_->OrderSysID,
              f_->OrderSubmitStatus,
              f_->OrderStatus,
              f_->InsertDate,
              f_->InsertTime,
              f_->LimitPrice,
              f_->VolumeTraded,
              f_->VolumeTotal );

    // https://www.cnblogs.com/leijiangtao/p/5379133.html
    switch ( f_->OrderStatus ) {
    default:
    case THOST_FTDC_OST_NoTradeQueueing:
    case THOST_FTDC_OST_NotTouched:
    case THOST_FTDC_OST_NoTradeNotQueueing:
    case THOST_FTDC_OST_Touched:
    case THOST_FTDC_OST_Unknown:
        // todo
        // return delegator()->update_ord( oid, ord_status::pending );
        return;

    case THOST_FTDC_OST_PartTradedQueueing:
    case THOST_FTDC_OST_AllTraded:
    case THOST_FTDC_OST_PartTradedNotQueueing: {  // todo
#if 0
        order o;

        o.id     = oid;
        o.qty    = f_->VolumeTraded;
        o.remark = f_->StatusMsg;

        o.dt.from_ctp( f_->InsertDate, f_->InsertTime, 0 );
        o.code = f_->InstrumentID;

        o.status = ord_status::dealt;
        LOG_INFO( "order dir %d %s", f_->Direction, f_->CombOffsetFlag );

        o.dir = cvt_direction( f_->Direction, f_->CombOffsetFlag );
        if ( ord_dir::none == o.dir ) {
            LOG_INFO( "bad order direction'" );
        }

        o.price = f_->LimitPrice;

        // todo
        // return delegator()->update_ord( o );
#endif
        return;
    } break;

    case THOST_FTDC_OST_Canceled:
        LOG_INFO( "撤单原因: %d", f_->OrderSubmitStatus );
        // todo
        // return delegator()->update_ord( oid, ord_status::cancelled );
        return;
    }
}

void trader::OnRtnTrade( CThostFtdcTradeField* f_ ) {
    auto id = _id.id_of( { _ss.front, _ss.conn, ordref( f_->OrderRef ) } );

    if ( !id ) {
        LOG_INFO( "rtn trade, cannot reg order id" );
        return;
    }

#if 0
    order o;

    o.id = id.value().fsr.ref.int_val();
    o.datetime.from_ctp( f_->TradeDate, f_->TradeTime, 0 );
    o.qty   = f_->Volume;
    o.code  = f_->InstrumentID;
    o.limit = f_->price;
    o.dir   = cvt_direction( f_->Direction, f_->OffsetFlag );

    if ( ord_dir::none == o.dir ) {
        LOG_INFO( "bad order direction'" );
    }
#endif
    // todo
    // delegator()->update_ord( id->fsr.ref.int_val(), ord_status::finished );
    qry_fund();
}

void trader::OnErrRtnOrderInsert( CThostFtdcInputOrderField* f_, CThostFtdcRspInfoField* pRspInfo ) {
    LOG_INFO( "INSERT ERROR,errid=%d msg=%s", pRspInfo->ErrorID, pRspInfo->ErrorMsg );

    auto id = _id.id_of( { _ss.front, _ss.conn, ordref( f_->OrderRef ) } );

    if ( !id ) {
        LOG_INFO( "errrtn insert, cannot reg order id" );
        return;
    }

    // todo
    // delegator()->update_ord( id->fsr.ref.int_val(), ord_status::error );
}

void trader::OnErrRtnOrderAction( CThostFtdcOrderActionField* f_, CThostFtdcRspInfoField* pRspInfo ) {
    LOG_INFO( "order action error: {frontid=%d ,sessionid=%d, oref=%d} ,{ex=%s sysid=%s} , instument=%s, msg=%s, req=%d",
              f_->FrontID,
              f_->SessionID,
              f_->OrderRef,
              f_->ExchangeID,
              f_->OrderSysID,
              f_->InstrumentID,
              pRspInfo->ErrorID,
              pRspInfo->ErrorMsg );

    LOG_INFO( "撤单被交易所拒绝, 原因: [ %d %s ]", pRspInfo->ErrorID, pRspInfo->ErrorMsg );

    auto id = _id.id_of( { _ss.front, _ss.conn, ordref( f_->OrderRef ) } );

    if ( !id ) {
        LOG_INFO( "rtn trade, cannot reg order id" );
        return;
    }

    // todo
    // delegator()->update_ord( id->fsr.ref.int_val(), ord_status::aborted );
}

void trader::OnRspQryInstrument( CThostFtdcInstrumentField* pInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void trader::OnRspQryTradingAccount( CThostFtdcTradingAccountField* pTradingAccount, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    LOG_INFO( "qry account,errid=%d msg=%s", pRspInfo->ErrorID, pRspInfo->ErrorMsg );
    pub::acct_msg f;

    f.available  = pTradingAccount->Available;
    f.commission = pTradingAccount->Commission;
    f.balance    = pTradingAccount->Balance;
    f.cprofit    = pTradingAccount->CloseProfit;
    f.pprofit    = pTradingAccount->PositionProfit;

    PUB( f );
}

void trader::OnRspQryOrder( CThostFtdcOrderField* pOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void trader::OnRspQryTrade( CThostFtdcTradeField* pTrade, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void trader::OnRspQryInvestorPositionDetail( CThostFtdcInvestorPositionDetailField* pInvestorPositionDetail, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void trader::OnRspQryInvestorPosition( CThostFtdcInvestorPositionField* pInvestorPosition, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void trader::OnRtnInstrumentStatus( CThostFtdcInstrumentStatusField* pInstrumentStatus ) {
}

void trader::OnRspQryInstrumentMarginRate( CThostFtdcInstrumentMarginRateField* pInstrumentMarginRate, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void trader::OnRspQryExchangeMarginRateAdjust( CThostFtdcExchangeMarginRateAdjustField* pExchangeMarginRateAdjust, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void trader::OnRspQryInstrumentCommissionRate( CThostFtdcInstrumentCommissionRateField* pInstrumentCommissionRate, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}
}  // namespace ctp
NVX_NS_END
