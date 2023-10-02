#include "ctp_trade_proxy.h"

#include "../log.hpp"
#include "comm.h"

CUB_NS_BEGIN
int CtpTrader::qry_fund() {
    return 0;
}

int CtpTrader::qry_marginrate() {
    return 0;
}

int CtpTrader::qry_commission() {
    CThostFtdcQryInstrumentCommissionRateField field = { 0 };

    CTP_COPY_SAFE( filed.BrokerID, _settings.broker.c_str() );
    CTP_COPY_SAFE( filed.InvestorID, _settings.broker.c_str() );
    // CTP_COPY_SAFE( filed.InstrumentID, _settings..c_str() );

    int reqId = CtpProxy::GetRequestId();
    wait( []() {} );

    int rt = _api->ReqQryInstrumentCommissionRate( &commissionReq, reqId );

    if ( !rt ) {
        return this->_sync_wait_full_return( csd, lck, cmsn );
    }
    else {
        return -1;
    }
}

int CtpTrader::qry_position() {
    return 0;
}

int CtpTrader::init() {
    TraceInfo( "RegisterSpi@注册ctp交易网关\n" );
    std::string flow = _settings.flow_path;
    TraceInfo( "RegisterSpi@创建交易网关数据流目录：{}\n", flow );
    TraceInfo( "RegisterSpi@[ctp]创建交易网关数据流目录：\n{}\n", flow );
    _api = CThostFtdcTraderApi::CreateFtdcTraderApi( flow.c_str() );

    TraceInfo( "RegisterSpi@{ctp}初始化交易网关:注册事件\n" );
    _api->RegisterSpi( this );                         // 注册事件类
    _api->SubscribePublicTopic( THOST_TERT_RESTART );  // 订阅公共流
    _api->SubscribePrivateTopic( THOST_TERT_QUICK );   // 订阅私有流
    TraceInfo( "RegisterSpi@{ctp}初始化交易网关:注册交易前端\n" );

    for ( auto& a : _settigns.frontend ) {
        // char *s = "tcp://180.168.146.187:10010";
        _api->RegisterFront( addr );  // 设置交易前置地址,设置多个，内部ctp可以自动切换

        TraceInfo( "RegisterSpi@[ctp] tfront={}\n", addr );
    }

    TraceInfo( "{ctp} init\n" );
    _api->Init();

    //_api.join

    return 0;
}

int CtpTrader::teardown() {
    TraceInfo( "LqTradeSvc,ReleaseSpi@释放ctp交易网关\n" );

    // ctp开发手册中建议的顺序，但是即便如此，如果连续的release，依然会导致程序死机
    _api->RegisterSpi( nullptr );
    _api->Release();

    return 0;
}

int CtpTrader::login() {
    LOG_INFO( "xLogin@登录交易网关:[b:{} u:{}]\n", this->broker(), this->usr() );

    CThostFtdcReqUserLoginField login_req = { 0 };
    memcpy( login_req.BrokerID, _settings.conn.broker.c_str(), std::min( _settings.conn.broker.length(), sizeof( login_req.BrokerID ) - 1 ) );
    memcpy( login_req.UserID, _settings.conn.user_name.c_str(), std::min( _settings.conn.user_name.length(), sizeof( login_req.UserID ) - 1 ) );
    memcpy( login_req.Password, _settings.conn.password.c_str(), std::min( _settings.conn.password.length(), sizeof( login_req.Password ) - 1 ) );

    auto req = req_id();

    int rt = this->_api->ReqUserLogin( &login_req, session );
    if ( !rt ) {
        LOG_INFO( "i> LqSvcMarket::xLogin sucessfully, sess=%u\n", ( unsigned )session );
        return 0;
    }
    else {
        LOG_INFO( "c> LqSvcMarket::xLogin failed\n" );
        return -1;
    }
}

int CtpTrader::logout() {
    CThostFtdcUserLogoutField logout_req = { 0 };
    memcpy( logout_req.BrokerID, _settings.conn.broker.c_str(), std::min( _settings.conn.broker.length(), sizeof( login_req.BrokerID ) - 1 ) );
    memcpy( logout_req.UserID, _settings.conn.user_name.c_str(), std::min( _settings.conn.user_name.length(), sizeof( login_req.UserID ) - 1 ) );

    auto req = req_id();
    int  rt  = _api->ReqUserLogout( &logout_req, req );
    if ( !rt ) {
        return reqId;
    }
    else {
        return -1;
    }
}

void CtpTrader::OnRtnBulletin( CThostFtdcBulletinField* pBulletin ) {
}

void CtpTrader::OnRtnTradingNotice( CThostFtdcTradingNoticeInfoField* pTradingNoticeInfo ) {
}

int CtpTrader::auth() {
    TraceInfo( "xReqAuth@auth client:[ {}, {}]\n", this->appid(), this->auth() );

    CThostFtdcReqAuthenticateField field = { 0 };
    memcpy( field.BrokerID, _settings.conn.broker.c_str(), std::min( _settings.conn.broker.length(), sizeof( field.BrokerID ) - 1 ) );
    memcpy( field.UserID, _settings.conn.user_name.c_str(), std::min( _settings.conn.user_name.length(), sizeof( field.UserID ) - 1 ) );
    strcpy_s( field.AppID, _settings.app_id.c_str(), std::min( _settings.app_id.length(), sizeof( field.AppID ) - 1 ) );
    strcpy_s( field.AuthCode, _settings.auth_code.c_str(), std::min( _settings.auth_code.length(), sizeof( field.AuthCode ) - 1 ) );

    auto req = req_id();
    if ( 0 != _api->ReqAuthenticate( &field, req ) ) {

        TraceInfo( "send auth req faild,try again later\n" );
        return -1;
    }

    return 0;
}

void CtpTrader::OnFrontConnected() {
    LOG_INFO( ">> 交易网关链接成功\n" );
    auth();
}

int CtpTrader::qry_settlement() {
    CThostFtdcQrySettlementInfoField field = { 0 };
    memcpy( field.BrokerID, _settings.conn.broker.c_str(), std::min( _settings.conn.broker.length(), sizeof( field.BrokerID ) - 1 ) );
    memcpy( field.InvestorID, _settings.conn.user_name.c_str(), std::min( _settings.conn.user_name.length(), sizeof( field.InvestorID ) - 1 ) );

    /*不能带有日期参数，否则会失败*/
    // strcpy(settleInfoReq.TradingDay, m_tradingDay);

    _settle_req = req_id();

    return 0 != _api->ReqQrySettlementInfo( &field, _settle_req ) ? 0 : -1;
}

int CtpTrader::confirm_settlement() {
    CThostFtdcSettlementInfoConfirmField field = { 0 };

    memcpy( field.BrokerID, _settings.conn.broker.c_str(), std::min( _settings.conn.broker.length(), sizeof( field.BrokerID ) - 1 ) );
    memcpy( field.InvestorID, _settings.conn.user_name.c_str(), std::min( _settings.conn.user_name.length(), sizeof( field.InvestorID ) - 1 ) );

    return _api->ReqSettlementInfoConfirm( &settlementConfirmReq, req_id() ) != 0 ? 0 : -1;
}

void CtpTrader::OnRspAuthenticate( CThostFtdcRspAuthenticateField* pRspAuthenticateField, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    TraceInfo( "OnRspAuthenticate\n" );

    if ( pRspInfo != NULL && pRspInfo->ErrorID == 0 ) {
        TraceInfo( "认证成功,ErrorID={:x}, ErrMsg={}\n\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg );
        login();
    }
    else {
        TraceInfo( "认证失败,ErrorID={:x}, ErrMsg={}\n\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg );
    }
}

void CtpTrader::OnRspUserLogin( CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    qry_settlement();
}

void CtpTrader::OnRspError( CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpTrader::OnFrontDisconnected( int nReason ) {
}

void CtpTrader::OnHeartBeatWarning( int nTimeLapse ) {
}

void CtpTrader::OnRspUserLogout( CThostFtdcUserLogoutField* pUserLogout, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpTrader::OnRspSettlementInfoConfirm( CThostFtdcSettlementInfoConfirmField* pSettlementInfoConfirm, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    TraceInfo( "投资者结算结果确认成功,确认日期：{}-{}\n", f->ConfirmDate, f->ConfirmTime );
}

void CtpTrader::OnRspQrySettlementInfo( CThostFtdcSettlementInfoField* pSettlementInfo, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    LOG_INFO( "settlement received" );
    if ( bIsLast ) {
        confirm_settlement();
    }
}

void CtpTrader::OnRspQryInstrument( CThostFtdcInstrumentField* pInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpTrader::OnRspQryTradingAccount( CThostFtdcTradingAccountField* pTradingAccount, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpTrader::OnRspQryOrder( CThostFtdcOrderField* pOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpTrader::OnRspQryTrade( CThostFtdcTradeField* pTrade, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpTrader::OnRspQryInvestorPositionDetail( CThostFtdcInvestorPositionDetailField* pInvestorPositionDetail, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpTrader::OnRspQryInvestorPosition( CThostFtdcInvestorPositionField* pInvestorPosition, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpTrader::OnRspOrderInsert( CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpTrader::OnRspOrderAction( CThostFtdcInputOrderActionField* pInputOrderAction, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpTrader::OnRtnOrder( CThostFtdcOrderField* pOrder ) {
}

void CtpTrader::OnErrRtnOrderInsert( CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo ) {
}

void CtpTrader::OnErrRtnOrderAction( CThostFtdcOrderActionField* pOrderAction, CThostFtdcRspInfoField* pRspInfo ) {
}

void CtpTrader::OnRtnTrade( CThostFtdcTradeField* pTrade ) {
}

void CtpTrader::OnRtnInstrumentStatus( CThostFtdcInstrumentStatusField* pInstrumentStatus ) {
}

void CtpTrader::OnRspQryInstrumentMarginRate( CThostFtdcInstrumentMarginRateField* pInstrumentMarginRate, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpTrader::OnRspQryExchangeMarginRateAdjust( CThostFtdcExchangeMarginRateAdjustField* pExchangeMarginRateAdjust, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpTrader::OnRspQryInstrumentCommissionRate( CThostFtdcInstrumentCommissionRateField* pInstrumentCommissionRate, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

CUB_NS_END