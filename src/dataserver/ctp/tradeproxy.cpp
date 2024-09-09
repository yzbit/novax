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

* \author: yaozn(zinan@outlook.com), qianq(695997058@qq.com)
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
        LOG_INFO( "ctp", "load config failed: %s", CTP_TRADE_SETTING_FILE );
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

void trader::reconnected( const session& s_, const ordref& max_ref_ ) {
    LOG_INFO( "recoonected: font=%d, conn=%d", s_.front, s_.conn );

    _ss = s_;

    if ( _last_ref < max_ref_ ) {
        LOG_INFO( "reset start id [%s] to [%s]", _last_ref.data(), max_ref_.data() );
        _last_ref = r;
    }

    // if ( !_settled ) {
    qry_settlement();
    //}
}

nxt_st trader::qry_instruments() {
    CTP_CHECK_READY( NVX_FAIL );

    CThostFtdcQryInstrumentField ins = {};

    auto rc = ReqQryInstrument( &ins, req_id );

    if ( 0 == rc ) {
        LOG_INFO( "qry ins sent" );
        return NVX_OK;
    }

    LOG_INFO( "qry ins fail=%d", rc );
    return NVX_FAIL;
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
    reconnected( { pRspUserLogin->FrontID, pRspUserLogin->SessionID }, { pRspUserLogin->MaxOrderRef } );
}

void trader::OnRspError( CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    LOG_INFO( "rsp error: id=%d msg=%s", pRspInfo->ErrorID, pRspInfo->ErrorMsg );
}

void trader::OnFrontDisconnected( int nReason ) {
    LOG_INFO( "front disconnted:reason=%d", nReason );
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

void trader::OnRspQryInstrument( CThostFtdcInstrumentField* pInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

}  // namespace ctp
NVX_NS_END
