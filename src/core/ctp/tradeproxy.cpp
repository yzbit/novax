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
namespace ctp {

CtpTrader::CtpTrader( IPub* p_ )
    : IBroker( p_ ) {}

nvx_st CtpTrader::start() {
    if ( _settings.load( CTP_TRADE_SETTING_FILE ) < 0 ) {
        LOG_TAGGED( "ctp", "load config failed: %s", CTP_TRADE_SETTING_FILE );
        return NVX_Fail;
    }

    LOG_INFO( "RegisterSpi@注册ctp交易网关\n" );
    std::string flow = _settings.flow_path;

    LOG_INFO( "RegisterSpi@创建交易网关数据流目录：{}\n", flow );
    LOG_INFO( "RegisterSpi@[ctp]创建交易网关数据流目录：\n{}\n", flow );
    _api = CThostFtdcTraderApi::CreateFtdcTraderApi( flow.c_str() );

    LOG_INFO( "RegisterSpi@{ctp}初始化交易网关:注册事件\n" );

    _api->RegisterSpi( this );                         // 注册事件类
    _api->SubscribePublicTopic( THOST_TERT_RESTART );  // 订阅公共流
    _api->SubscribePrivateTopic( THOST_TERT_QUICK );   // 订阅私有流

    LOG_INFO( "RegisterSpi@{ctp}初始化交易网关:注册交易前端\n" );

    for ( auto& a : _settings.frontend ) {
        // char *s = "tcp://180.168.146.187:10010";
        // todo 设置多个地址和一次性还是循环调用
        _api->RegisterFront( const_cast<char*>( a.c_str() ) );  // 设置交易前置地址,设置多个，内部ctp可以自动切换
        LOG_INFO( "RegisterSpi@[ctp] tfront=%s\n", a.c_str() );
    }

    LOG_INFO( "{ctp} init\n" );
    _api->Init();

    //_api.join

    return NVX_OK;
}

nvx_st CtpTrader::stop() {
    LOG_INFO( "LqTradeSvc,ReleaseSpi@释放ctp交易网关\n" );

    // ctp 手册建议:
    _api->RegisterSpi( nullptr );
    _api->Release();

    return NVX_OK;
}

nvx_st CtpTrader::qry_fund() {
    return NVX_OK;
}

nvx_st CtpTrader::qry_marginrate() {
    return NVX_OK;
}

nvx_st CtpTrader::qry_commission() {
    CThostFtdcQryInstrumentCommissionRateField field = { 0 };

    CTP_COPY_SAFE( field.BrokerID, _settings.i.broker.c_str() );
    CTP_COPY_SAFE( field.InvestorID, _settings.i.id.c_str() );
    // CTP_COPY_SAFE( filed.InstrumentID, _settings..c_str() );

    return !_api->ReqQryInstrumentCommissionRate( &field, req_id() ) ? NVX_OK
                                                                     : NVX_Fail;
}

void CtpTrader::session_changed( const session_t& s_ ) {
    // todo
    assert( 0 );
}

nvx_st CtpTrader::qry_position() {
    return NVX_OK;
}

nvx_st CtpTrader::cancel( const order_t& o_ ) {
    CThostFtdcInputOrderActionField field = { 0 };

    CTP_COPY_SAFE( field.BrokerID, _settings.i.broker.c_str() );
    CTP_COPY_SAFE( field.InvestorID, _settings.i.id.c_str() );
    // UserID非必填，是操作员账号，不填写会收不到OnErrRtnOrderAction回报，一般客户和InvestorID填写一样就可以
    CTP_COPY_SAFE( field.UserID, _settings.i.id.c_str() );

    /*来自网络的解释：
    如果单子还在CTP就要用FrontID+SessionID+OrderRef撤，因为送到exchange前还没有OrderSysID.
    如果单子已经送到Exchange,那既可以用 FrontID+SessionID+OrderRef撤，也可以用ExchangeID+OrderSysID撤。
    */
    auto id = id_of( o_.id );

    if ( !id ) {
        LOG_INFO( "xModifyOrder@试图删除一个不存在订单ID,%d\n", o_ );
        return NVX_Fail;
    }

    if ( id->has_sysid() ) {
        memcpy( field.OrderSysID, id->sysid.order_id, sizeof( field.OrderSysID ) );
        memcpy( field.ExchangeID, id->sysid.ex_id, sizeof( field.ExchangeID ) );
    }
    else {
        id->fsr.ref.copy( field.OrderRef );
        field.FrontID   = _ss.front;
        field.SessionID = _ss.sess;
    }

    CTP_COPY_SAFE( field.InstrumentID, o_.code.c_str() );

    field.ActionFlag = THOST_FTDC_AF_Delete;

    LOG_INFO( "-------------------------------ctp撤单参数----------------------------------\n" );
    LOG_INFO( "BrokerID = %s, InvestorID=%s\
        ExchangeID=%s,OrderSysID=%s\
        FrontID=%d,SessionID=%d,OrderRef=%s\
        ActionFlag=%d,InstrumentID=%s\n",
              field.BrokerID,
              field.InvestorID,
              field.ExchangeID,
              field.OrderSysID,
              field.FrontID,
              field.SessionID,
              field.OrderRef,
              field.ActionFlag,
              field.InstrumentID );

    // todo
    // 这里不能这么做，因为用户可能发出很多次req--比如在返回之前多次cancel，put,只能假设用户不会通过其他客户端也在操作
    return !_api->ReqOrderAction( &field, req_id() ) ? NVX_OK : NVX_Fail;
}

nvx_st CtpTrader::put( const order_t& o_ ) {
    LOG_INFO( "xPutOrder@ctp准备下单\n" );

    if ( !_api ) {
        LOG_INFO( "##ctp, not inited" );
        return NVX_Fail;
    }

    order_id_t id;
    id.fsr = fsr_t( _ss.front, _ss.sess, order_ref_t( o_.id ) );
    _ids.emplace_back( id );

    CThostFtdcInputOrderField field = { 0 };

    id.fsr.ref.copy( field.OrderRef );
    CTP_COPY_SAFE( field.BrokerID, _settings.i.broker.c_str() );
    CTP_COPY_SAFE( field.InvestorID, _settings.i.id.c_str() );
    CTP_COPY_SAFE( field.UserID, _settings.i.id.c_str() );

    memcpy( field.InstrumentID, o_.code.c_str(), o_.code.length() );
    field.IsSwapOrder         = 0;
    field.ForceCloseReason    = THOST_FTDC_FCC_NotForceClose;
    field.MinVolume           = 0;
    field.ContingentCondition = THOST_FTDC_CC_Immediately;
    field.VolumeTotalOriginal = ( TThostFtdcVolumeType )o_.qty;
    field.LimitPrice          = o_.price;
    field.CombHedgeFlag[ 0 ]  = THOST_FTDC_HF_Speculation;
    field.ForceCloseReason    = THOST_FTDC_FCC_NotForceClose;
    field.IsAutoSuspend       = 0;
    field.UserForceClose      = 0;

    LOG_INFO( "order mode:%d\n", o_.mode );

    switch ( o_.mode ) {
    default:
        LOG_INFO( "不存在的订单模式:%d\n", o_.mode );
        return NVX_Fail;

    case otype_t::market:
        field.OrderPriceType  = THOST_FTDC_OPT_AnyPrice;
        field.TimeCondition   = THOST_FTDC_TC_IOC;
        field.VolumeCondition = THOST_FTDC_VC_AV;
        field.LimitPrice      = 0;
        break;

    case otype_t::fak:
        field.OrderPriceType  = THOST_FTDC_OPT_LimitPrice;
        field.TimeCondition   = THOST_FTDC_TC_IOC;
        field.VolumeCondition = THOST_FTDC_VC_MV;
        field.MinVolume       = 1;
        break;

    case otype_t::fok:
        field.OrderPriceType  = THOST_FTDC_OPT_LimitPrice;
        field.TimeCondition   = THOST_FTDC_TC_IOC;
        field.VolumeCondition = THOST_FTDC_VC_CV;
        break;

    case otype_t::wok:
        field.OrderPriceType  = THOST_FTDC_OPT_LimitPrice;
        field.TimeCondition   = THOST_FTDC_TC_GFD;  //! 当日有效
        field.VolumeCondition = THOST_FTDC_VC_AV;
        break;
    }

    LOG_INFO( "order direction: %d", o_.dir );

    switch ( o_.dir ) {
    default:
        LOG_INFO( "bad order direction: %d", o_.dir );
        return NVX_Fail;

    case odir_t::p_long:
        field.Direction           = THOST_FTDC_D_Buy;
        field.CombOffsetFlag[ 0 ] = THOST_FTDC_OF_Open;
        break;

    case odir_t::sell:
        field.Direction           = THOST_FTDC_D_Sell;
        field.CombOffsetFlag[ 0 ] = THOST_FTDC_OF_Close;
        break;

    case odir_t::p_short:
        field.Direction           = THOST_FTDC_D_Sell;
        field.CombOffsetFlag[ 0 ] = THOST_FTDC_OF_Open;
        break;

    case odir_t::cover:
        field.Direction           = THOST_FTDC_D_Buy;
        field.CombOffsetFlag[ 0 ] = THOST_FTDC_OF_Close;
        break;
    }

    LOG_INFO( "---------------------------ctp下单参数------------------------------------------\n" );
    LOG_INFO( "\nbroker=%s\ninvestor=%s\ncode=%s\nref=%s\n\
    LimitPrice=%lf\nVolumeTotalOriginal=%d\nContingentCondition=%d\n\
    MinVolume=%d\nOrderPriceType=%d\nTimeCondition=%d\n\
    VolumeCondition=%d\nDirection=%d\n\
    CombOffsetFlag[0]=%d\nCombHedgeFlag=%d\n\
    ForceCloseReason=%d\nIsAutoSuspend=%d\nUserForceClose=%d\n",
              field.BrokerID,
              field.InvestorID,
              field.InstrumentID,
              field.OrderRef,
              field.LimitPrice,
              field.VolumeTotalOriginal,
              field.ContingentCondition,
              field.MinVolume,
              field.OrderPriceType,
              field.TimeCondition,
              field.VolumeCondition,
              field.Direction,
              field.CombOffsetFlag,
              field.CombHedgeFlag,
              field.ForceCloseReason,
              field.IsAutoSuspend,
              field.UserForceClose );

    field.RequestID = req_id();

    return !_api->ReqOrderInsert( &field, field.RequestID ) ? NVX_OK : NVX_Fail;
}

nvx_st CtpTrader::login() {
    LOG_INFO( "xLogin@登录交易网关:%s %s\n", _settings.i.broker.c_str(), _settings.i.name.c_str() );

    CThostFtdcReqUserLoginField field = { 0 };

    CTP_COPY_SAFE( field.BrokerID, _settings.i.broker.c_str() );
    CTP_COPY_SAFE( field.UserID, _settings.i.name.c_str() );
    CTP_COPY_SAFE( field.Password, _settings.i.password.c_str() );

    return !this->_api->ReqUserLogin( &field, req_id() ) ? NVX_OK : NVX_Fail;
}

nvx_st CtpTrader::logout() {
    CThostFtdcUserLogoutField field = { 0 };

    CTP_COPY_SAFE( field.BrokerID, _settings.i.broker.c_str() );
    CTP_COPY_SAFE( field.UserID, _settings.i.name.c_str() );

    return !_api->ReqUserLogout( &field, req_id() ) ? NVX_OK : NVX_Fail;
}

void CtpTrader::OnRtnBulletin( CThostFtdcBulletinField* pBulletin ) {
}

void CtpTrader::OnRtnTradingNotice( CThostFtdcTradingNoticeInfoField* pTradingNoticeInfo ) {
}

nvx_st CtpTrader::auth() {
    LOG_INFO( "xReqAuth@auth client:%s %s %s\n", _settings.c.auth.c_str(), _settings.c.appid.c_str(), _settings.c.token.c_str() );

    CThostFtdcReqAuthenticateField field = { 0 };

    CTP_COPY_SAFE( field.BrokerID, _settings.i.broker.c_str() );
    CTP_COPY_SAFE( field.UserID, _settings.i.name.c_str() );
    CTP_COPY_SAFE( field.AppID, _settings.c.appid.c_str() );
    CTP_COPY_SAFE( field.AuthCode, _settings.c.token.c_str() );

    return !_api->ReqAuthenticate( &field, req_id() ) ? NVX_OK : NVX_Fail;
}

void CtpTrader::OnFrontConnected() {
    LOG_INFO( ">> 交易网关链接成功\n" );
    auth();
}

nvx_st CtpTrader::qry_settlement() {
    CThostFtdcQrySettlementInfoField field = { 0 };
    CTP_COPY_SAFE( field.BrokerID, _settings.i.broker.c_str() );
    CTP_COPY_SAFE( field.InvestorID, _settings.i.id.c_str() );

    /*! 不能带有日期参数，否则会失败*/
    // strcpy(settleInfoReq.TradingDay, m_tradingDay);
    auto req = req_id();
    auto rc  = CTP_SYNC.wait( req, 2000, _api, &CThostFtdcTraderApi::ReqQrySettlementInfo, &field, req );

    if ( rc.size() > 0 ) {
        return confirm_settlement();
    }

    return NVX_Fail;
}

nvx_st CtpTrader::confirm_settlement() {
    CThostFtdcSettlementInfoConfirmField field = { 0 };

    CTP_COPY_SAFE( field.BrokerID, _settings.i.broker.c_str() );
    CTP_COPY_SAFE( field.InvestorID, _settings.i.id.c_str() );

    return _api->ReqSettlementInfoConfirm( &field, req_id() ) != 0 ? NVX_OK : NVX_Fail;
}

void CtpTrader::OnRspAuthenticate( CThostFtdcRspAuthenticateField* pRspAuthenticateField, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    LOG_INFO( "OnRspAuthenticate\n" );

    if ( pRspInfo != NULL && pRspInfo->ErrorID == 0 ) {
        LOG_INFO( "认证成功,ErrorID={:x}, ErrMsg={}\n\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg );
        login();
    }
    else {
        LOG_INFO( "认证失败,ErrorID={:x}, ErrMsg={}\n\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg );
    }
}

void CtpTrader::OnRspUserLogin( CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    session_changed( { pRspUserLogin->FrontID, pRspUserLogin->SessionID, pRspUserLogin->MaxOrderRef } );
    // todo
    // tune_clock();  // todo
    if ( qry_settlement() < 0 ) {
        LOG_TAGGED( "ctp", "qry settlement failed" );
    }
}

void CtpTrader::OnRspError( CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    LOG_INFO( "rsp error: id=%d msg=%s", pRspInfo->ErrorID, pRspInfo->ErrorMsg );
}

void CtpTrader::OnFrontDisconnected( int nReason ) {
    LOG_INFO( "front disconnted:reason=%d", nReason );
}

void CtpTrader::OnHeartBeatWarning( int nTimeLapse ) {
}

void CtpTrader::OnRspUserLogout( CThostFtdcUserLogoutField* pUserLogout, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    LOG_ERROR_AND_RET( pRspInfo, nRequestID, bIsLast );

    LOG_INFO( "user logout %s", pUserLogout->UserID );
}

void CtpTrader::OnRspSettlementInfoConfirm( CThostFtdcSettlementInfoConfirmField* f, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    LOG_INFO( "投资者结算结果确认成功,确认日期：%s %s", f->ConfirmDate, f->ConfirmTime );
}

void CtpTrader::OnRspQrySettlementInfo( CThostFtdcSettlementInfoField* pSettlementInfo, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    LOG_INFO( "settlement received, req=%d last=%d", nRequestID, bIsLast );
    CTP_SYNC.update( nRequestID, pSettlementInfo, sizeof( CThostFtdcSettlementInfoField ), bIsLast );
}

void CtpTrader::OnRspOrderInsert( CThostFtdcInputOrderField* f_, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    LOG_INFO( "订单被ctp前置拒绝,req=%d, id=%d, msg=%s", nRequestID, pRspInfo->ErrorID, pRspInfo->ErrorMsg );
    auto id = id_of( { _ss.front, _ss.sess, order_ref_t( f_->OrderRef ) } );

    if ( !id ) {
        LOG_INFO( "can not reg order ref: %s", f_->OrderRef );
        return;
    }

    // todo
    // delegator()->update_ord( id->fsr.ref.int_val(), ostatus_t::cancelled );
}

void CtpTrader::OnRspOrderAction( CThostFtdcInputOrderActionField* f_, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    LOG_INFO( "撤单被ctp前置拒绝,req=%d, id=%d, msg=%s", nRequestID, pRspInfo->ErrorID, pRspInfo->ErrorMsg );

    auto id = id_of( { _ss.front, _ss.sess, order_ref_t( f_->OrderRef ) } );

    if ( !id ) {
        LOG_INFO( "rtn trade, cannot reg order id" );
        return;
    }

    // todo
    //  delegator()->update_ord( id->fsr.ref.int_val(), ostatus_t::aborted );
}

odir_t CtpTrader::cvt_direction( const TThostFtdcDirectionType& di_, const TThostFtdcCombOffsetFlagType& comb_ ) {
    if ( di_ == THOST_FTDC_D_Buy && comb_[ 0 ] == '0' )
        return odir_t::p_long;
    else if ( di_ == THOST_FTDC_D_Buy && comb_[ 0 ] == '1' )
        return odir_t::cover;
    else if ( di_ == THOST_FTDC_D_Sell && comb_[ 0 ] == '0' )
        return odir_t::p_short;
    else if ( di_ == THOST_FTDC_D_Sell && comb_[ 0 ] == '1' )
        return odir_t::sell;
    else
        return odir_t::none;
}

odir_t CtpTrader::cvt_direction( const TThostFtdcDirectionType& di_, const TThostFtdcOffsetFlagType& comb_ ) {
    bool open = comb_ == THOST_FTDC_OF_Open;

    if ( di_ == THOST_FTDC_D_Buy && open )
        return odir_t::p_long;
    else if ( di_ == THOST_FTDC_D_Buy && !open )
        return odir_t::cover;
    else if ( di_ == THOST_FTDC_D_Sell && open )
        return odir_t::p_short;
    else if ( di_ == THOST_FTDC_D_Sell && !open )
        return odir_t::sell;
    else
        return odir_t::none;
}

order_id_t* CtpTrader::id_of( const oid_t& oid_ ) {
    return 0;
}

order_id_t* CtpTrader::id_of( const fsr_t& ref_ ) {
    // auto oid = std::find( _ids.begin(), _ids.end(), [ & ]( const auto& id_ ) { return id_.fsr.ref == ref_; } );
    // return oid == _ids.end() ? nullptr : &( *oid );
    return 0;
}

order_id_t* CtpTrader::id_of( const sys_order_t& sys_ ) {
    // auto oid = std::find( _ids.begin(), _ids.end(), [ & ]( const auto& id_ ) { return id_.sysid == sys_; } );
    // return oid == _ids.end() ? nullptr : &( *oid );
    return 0;
}

void CtpTrader::OnRtnOrder( CThostFtdcOrderField* f_ ) {
    LOG_INFO( "[ctp] OnOrderRtn[1]@front=%d sess=%d ref=%s exid=%s sysid=%s\n", f_->FrontID, f_->SessionID, f_->OrderRef, f_->ExchangeID, f_->OrderSysID );

    order_id_t* id = f_->RelativeOrderSysID[ 0 ] != 0
                         ? id_of( { f_->ExchangeID, f_->RelativeOrderSysID } )
                         : id_of( { f_->FrontID, f_->SessionID, order_ref_t( f_->OrderRef ) } );

    if ( !id ) {
        LOG_INFO( "cannot reg order id ex= %s syso=%d ref=%s rel=%s", f_->ExchangeID, f_->OrderSysID, f_->OrderRef, f_->RelativeOrderSysID );
        return;
    }

    oid_t oid = id->fsr.ref.int_val();
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
        // return delegator()->update_ord( oid, ostatus_t::pending );
        return;

    case THOST_FTDC_OST_PartTradedQueueing:
    case THOST_FTDC_OST_AllTraded:
    case THOST_FTDC_OST_PartTradedNotQueueing: {  // todo
        order_t o;

        o.id     = oid;
        o.qty    = f_->VolumeTraded;
        o.remark = f_->StatusMsg;

        o.datetime.from_ctp( f_->InsertDate, f_->InsertTime, 0 );
        o.code = f_->InstrumentID;

        o.status = ostatus_t::dealt;
        LOG_INFO( "order dir %d %s", f_->Direction, f_->CombOffsetFlag );

        o.dir = cvt_direction( f_->Direction, f_->CombOffsetFlag );
        if ( odir_t::none == o.dir ) {
            LOG_INFO( "bad order direction'" );
        }

        o.price = f_->LimitPrice;

        // todo
        // return delegator()->update_ord( o );
        return;
    } break;

    case THOST_FTDC_OST_Canceled:
        LOG_INFO( "撤单原因: %d", f_->OrderSubmitStatus );
        // todo
        // return delegator()->update_ord( oid, ostatus_t::cancelled );
        return;
    }
}

void CtpTrader::OnRtnTrade( CThostFtdcTradeField* f_ ) {
    auto id = id_of( { _ss.front, _ss.sess, order_ref_t( f_->OrderRef ) } );

    if ( !id ) {
        LOG_INFO( "rtn trade, cannot reg order id" );
        return;
    }

    order_t o;

    o.id = id->fsr.ref.int_val();
    o.datetime.from_ctp( f_->TradeDate, f_->TradeTime, 0 );
    o.qty   = f_->Volume;
    o.code  = f_->InstrumentID;
    o.price = f_->Price;
    o.dir   = cvt_direction( f_->Direction, f_->OffsetFlag );

    if ( odir_t::none == o.dir ) {
        LOG_INFO( "bad order direction'" );
    }

    // todo
    // delegator()->update_ord( id->fsr.ref.int_val(), ostatus_t::finished );
    qry_fund();
}

void CtpTrader::OnErrRtnOrderInsert( CThostFtdcInputOrderField* f_, CThostFtdcRspInfoField* pRspInfo ) {
    LOG_INFO( "INSERT ERROR,errid=%d msg=%s", pRspInfo->ErrorID, pRspInfo->ErrorMsg );

    auto id = id_of( { _ss.front, _ss.sess, order_ref_t( f_->OrderRef ) } );

    if ( !id ) {
        LOG_INFO( "errrtn insert, cannot reg order id" );
        return;
    }

    // todo
    // delegator()->update_ord( id->fsr.ref.int_val(), ostatus_t::error );
}

void CtpTrader::OnErrRtnOrderAction( CThostFtdcOrderActionField* f_, CThostFtdcRspInfoField* pRspInfo ) {
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

    auto id = id_of( { _ss.front, _ss.sess, order_ref_t( f_->OrderRef ) } );

    if ( !id ) {
        LOG_INFO( "rtn trade, cannot reg order id" );
        return;
    }

    // todo
    // delegator()->update_ord( id->fsr.ref.int_val(), ostatus_t::aborted );
}

void CtpTrader::OnRspQryInstrument( CThostFtdcInstrumentField* pInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpTrader::OnRspQryTradingAccount( CThostFtdcTradingAccountField* pTradingAccount, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    LOG_INFO( "qry account,errid=%d msg=%s", pRspInfo->ErrorID, pRspInfo->ErrorMsg );
    fund_t f;

    f.available  = pTradingAccount->Available;
    f.commission = pTradingAccount->Commission;
    f.balance    = pTradingAccount->Balance;
    f.cprofit    = pTradingAccount->CloseProfit;
    f.pprofit    = pTradingAccount->PositionProfit;

    fund_msg_t m;
    m.f = f;

    PUB_MSG( m );
}

void CtpTrader::OnRspQryOrder( CThostFtdcOrderField* pOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpTrader::OnRspQryTrade( CThostFtdcTradeField* pTrade, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpTrader::OnRspQryInvestorPositionDetail( CThostFtdcInvestorPositionDetailField* pInvestorPositionDetail, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpTrader::OnRspQryInvestorPosition( CThostFtdcInvestorPositionField* pInvestorPosition, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpTrader::OnRtnInstrumentStatus( CThostFtdcInstrumentStatusField* pInstrumentStatus ) {
}

void CtpTrader::OnRspQryInstrumentMarginRate( CThostFtdcInstrumentMarginRateField* pInstrumentMarginRate, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpTrader::OnRspQryExchangeMarginRateAdjust( CThostFtdcExchangeMarginRateAdjustField* pExchangeMarginRateAdjust, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpTrader::OnRspQryInstrumentCommissionRate( CThostFtdcInstrumentCommissionRateField* pInstrumentCommissionRate, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}
}  // namespace ctp
NVX_NS_END
