#include "ctp_trade_proxy.h"

#include "../log.hpp"
#include "comm.h"

CUB_NS_BEGIN
namespace ctp {

CtpTrader::CtpTrader( OrderMgmt* om_ )
    : _om( om_ ) {}

int CtpTrader::start() {
    if ( _settings.load( CTP_TRADE_SETTING_FILE ) < 0 ) {
        LOG_TAGGED( "ctp", "load config failed: %s", CTP_TRADE_SETTING_FILE );
        return -1;
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

    return 0;
}

int CtpTrader::stop() {
    LOG_INFO( "LqTradeSvc,ReleaseSpi@释放ctp交易网关\n" );

    // ctp 手册建议:
    _api->RegisterSpi( nullptr );
    _api->Release();

    return 0;
}

int CtpTrader::qry_fund() {
    return 0;
}

int CtpTrader::qry_marginrate() {
    return 0;
}

int CtpTrader::qry_commission() {
    CThostFtdcQryInstrumentCommissionRateField field = { 0 };

    CTP_COPY_SAFE( field.BrokerID, _settings.i.broker.c_str() );
    CTP_COPY_SAFE( field.InvestorID, _settings.i.id.c_str() );
    // CTP_COPY_SAFE( filed.InstrumentID, _settings..c_str() );

    return !_api->ReqQryInstrumentCommissionRate( &field, req_id() ) ? 0
                                                                     : -1;
}

void CtpTrader::session_changed( const session_t& s_ ) {
    // todo
    assert( 0 );
}

int CtpTrader::qry_position() {
    return 0;
}

int CtpTrader::cancel( oid_t o_ ) {
    CThostFtdcInputOrderActionField field = { 0 };

    CTP_COPY_SAFE( field.BrokerID, _settings.i.broker.c_str() );
    CTP_COPY_SAFE( field.InvestorID, _settings.i.id.c_str() );

    /*来自网络的解释：
    如果单子还在CTP就要用FrontID+SessionID+OrderRef撤，因为送到exchange前还没有OrderSysID.
    如果单子已经送到Exchange,那既可以用 FrontID+SessionID+OrderRef撤，也可以用ExchangeID+OrderSysID撤。
    */
    auto oids = _id_map.find( o_ );

    if ( oids == _id_map.end() ) {
        LOG_INFO( "xModifyOrder@试图删除一个不存在订单ID,%d\n", o_ );
        return -1;
    }

    if ( oids->second.eoid.is_valid() ) {
        memcpy( field.OrderSysID, oids->second.eoid.oid, sizeof( field.OrderSysID ) );
        memcpy( field.ExchangeID, oids->second.eoid.ex, sizeof( field.ExchangeID ) );
    }
    else if ( IS_VALID_REF( oids->second.ref ) ) {
        memcpy( field.OrderRef, oids->second.ref, sizeof( field.OrderRef ) );
        field.FrontID   = _ss.front;
        field.SessionID = _ss.sess;
    }
    else {
        LOG_INFO( "xModifyOrder@试图删除一个不存在订单ID,oid=%d\n", o_ );
        return -2;
    }

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
    return !_api->ReqOrderAction( &field, req_id() ) ? 0 : -1;
}

int CtpTrader::put( const order_t& o_ ) {
    LOG_INFO( "xPutOrder@ctp准备下单\n" );

    CThostFtdcInputOrderField field = { 0 };
    CTP_COPY_SAFE( field.BrokerID, _settings.i.broker.c_str() );
    CTP_COPY_SAFE( field.InvestorID, _settings.i.id.c_str() );
    memcpy( field.InstrumentID, o_.code, sizeof( o_.code ) );

    // todo 如果是平仓，还需要给id吗?
    if ( assign_ref( o_.id ) < 0 ) {
        LOG_INFO( "cant gen order ref or id%u", o_.id );
        return -1;
    }

    memcpy( field.OrderRef, _id_map[ o_.id ].ref, sizeof( field.OrderRef ) );

    field.MinVolume           = 0;                               /// 最小成交量: 1-fak--!!
    field.ContingentCondition = THOST_FTDC_CC_Immediately;       /// 触发条件: 立即
    field.VolumeTotalOriginal = ( TThostFtdcVolumeType )o_.qty;  /// 数量:1
    field.LimitPrice          = o_.price;

    LOG_INFO( "单模式:%d\n", o_.mode );

    switch ( o_.mode ) {
    default:
        LOG_INFO( "不存在的订单模式:%d\n", o_.mode );
        return -1;

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
        field.TimeCondition   = THOST_FTDC_TC_IOC;  //! 立即成交否则撤单
        field.VolumeCondition = THOST_FTDC_VC_CV;
        break;

    case otype_t::wok:
        field.OrderPriceType  = THOST_FTDC_OPT_LimitPrice;
        field.TimeCondition   = THOST_FTDC_TC_GFD;  //! 当日有效
        field.VolumeCondition = THOST_FTDC_VC_AV;
        break;
    }

    LOG_INFO( "order direction: %d", o_.dir );

    /*上期所的持仓分今仓（当日开仓）和昨仓（历史持仓），平仓时需要指定是平今仓还是昨仓。
     若 对 上 期 所 的 持 仓 直 接 使 用 THOST_FTDC_OF_Close ， 则 效 果 同 使 用 THOST_FTDC_OF_CloseYesterday 。
     若 对 其 他 交 易 所 的 持 仓 使 用 了 THOST_FTDC_OF_CloseToday 或 THOST_FTDC_OF_CloseYesterday，则效果同使用 THOST_FTDC_OF_Close。

     但是从ctp的手册中看，如果上期所只用close，他只是优先平昨仓，然后再平今（平仓顺序例子）
     另外,根据仓位查询章节可以知道： YdPosition 表示昨日收盘时持仓数量（≠ 当前的昨仓数量，静态，日间不随着开平仓而变化）
     */
    switch ( o_.dir ) {
    default:
        LOG_INFO( "bad order direction: %d", o_.dir );
        return -1;

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

    field.CombHedgeFlag[ 0 ] = THOST_FTDC_HF_Speculation;     /// 组合投机套保标志
    field.ForceCloseReason   = THOST_FTDC_FCC_NotForceClose;  /// 强平原因: 非强平
    field.IsAutoSuspend      = 0;                             /// 自动挂起标志: 否
    field.UserForceClose     = 0;                             /// 用户强评标志: 否

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

    return !_api->ReqOrderInsert( &field, field.RequestID ) ? 0 : -1;
}

int CtpTrader::assign_ref( oid_t id_ ) {
    CUB_ASSERT( id_ );

    if ( _id_map.find( id_ ) != _id_map.end() ) {
        LOG_INFO( "band order id" );
        return -1;
    }

    order_ids_t ids;

    int i = sizeof( TThostFtdcOrderRefType ) - 1;
    int j = 0;

    while ( _ss.init_ref[ j ] == '0' )
        ++j;
    --j;

    int carry = 0;
    for ( ; i > j; --i ) {
        if ( _ss.init_ref[ i ] == '9' ) {
            _ss.init_ref[ i ] == '0';
            carry = 1;
        }
        else {
            _ss.init_ref[ i ] += 1;
            carry = 0;
            break;
        }
    }

    if ( carry ) {
        CUB_ASSERT( i == j );
        _ss.init_ref[ i ] = '1';
    }

    ids.set_ref( _ss.init_ref );
    ids.id = id_;
    _id_map.emplace( id_, ids );

    return 0;
}

int CtpTrader::login() {
    LOG_INFO( "xLogin@登录交易网关:%s %s\n", _settings.i.broker.c_str(), _settings.i.name.c_str() );

    CThostFtdcReqUserLoginField field = { 0 };

    CTP_COPY_SAFE( field.BrokerID, _settings.i.broker.c_str() );
    CTP_COPY_SAFE( field.UserID, _settings.i.name.c_str() );
    CTP_COPY_SAFE( field.Password, _settings.i.password.c_str() );

    return !this->_api->ReqUserLogin( &field, req_id() ) ?: 0 - 1;
}

int CtpTrader::logout() {
    CThostFtdcUserLogoutField field = { 0 };

    CTP_COPY_SAFE( field.BrokerID, _settings.i.broker.c_str() );
    CTP_COPY_SAFE( field.UserID, _settings.i.name.c_str() );

    return !_api->ReqUserLogout( &field, req_id() ) ? 0 : -1;
}

void CtpTrader::OnRtnBulletin( CThostFtdcBulletinField* pBulletin ) {
}

void CtpTrader::OnRtnTradingNotice( CThostFtdcTradingNoticeInfoField* pTradingNoticeInfo ) {
}

int CtpTrader::auth() {
    LOG_INFO( "xReqAuth@auth client:%s %s %s\n", _settings.c.auth.c_str(), _settings.c.appid.c_str(), _settings.c.token.c_str() );

    CThostFtdcReqAuthenticateField field = { 0 };

    CTP_COPY_SAFE( field.BrokerID, _settings.i.broker.c_str() );
    CTP_COPY_SAFE( field.UserID, _settings.i.name.c_str() );
    CTP_COPY_SAFE( field.AppID, _settings.c.appid.c_str() );
    CTP_COPY_SAFE( field.AuthCode, _settings.c.token.c_str() );

    return !_api->ReqAuthenticate( &field, req_id() ) ? 0 : -1;
}

void CtpTrader::OnFrontConnected() {
    LOG_INFO( ">> 交易网关链接成功\n" );
    auth();
}

int CtpTrader::qry_settlement() {
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

    return -1;
}

int CtpTrader::confirm_settlement() {
    CThostFtdcSettlementInfoConfirmField field = { 0 };

    CTP_COPY_SAFE( field.BrokerID, _settings.i.broker.c_str() );
    CTP_COPY_SAFE( field.InvestorID, _settings.i.id.c_str() );

    return _api->ReqSettlementInfoConfirm( &field, req_id() ) != 0 ? 0 : -1;
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

// ctp文档：
// 报单录入请求响应，当执行ReqOrderInsert后有字段填写不对之类的CTP报错则通过此接口返回

// 实测:
/*报单录入--实际上就是委托成功  OnRspOrderInsert
fak fok好像不会到这里来；这两种模式会直接撤单，并且返回status='a' unkonwn
*/
// 理解： 只用该函数来处理错误即可
// 报单录入请求响应，当执行ReqOrderInsert后有字段填写不对之类的CTP报错则通过此接口返回
//
// 被ctp拒单后
// OnRspOrderInsert是当前报单者收到的回调，OnErrRtnOrderInsert是该客户名下所有的链接都会收到的回调。
// 而被交易所拒单后只会回调 OnRtnOrder
void CtpTrader::OnRspOrderInsert( CThostFtdcInputOrderField* f_, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    LOG_INFO( "订单被ctp前置拒绝,req=%d, id=%d, msg=%s", nRequestID, pRspInfo->ErrorID, pRspInfo->ErrorMsg );
    auto id = id_of( f_->OrderRef );

    if ( !IS_VALID_ID( id ) ) {
        LOG_INFO( "can not reg order ref: %s", f_->OrderRef );
        return;
    }

    _om->update( id, ostatus_t::cancelled );
}

// 报单操作请求响应，当执行ReqOrderAction后有字段填写不对之类的CTP报错则通过此接口返回
void CtpTrader::OnRspOrderAction( CThostFtdcInputOrderActionField* pInputOrderAction, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
    LOG_INFO( "撤单被ctp前置拒绝,req=%d, id=%d, msg=%s", nRequestID, pRspInfo->ErrorID, pRspInfo->ErrorMsg );
}

oid_t CtpTrader::id_of( const TThostFtdcOrderRefType& ref_ ) {
    auto itr = std::find_if( _id_map.begin(), _id_map.end(), [ & ]( auto& pair_ ) {
        return 0 == memcmp( ref_, pair_.second.ref, sizeof( ref_ ) ) == 0;
    } );

    return itr == _id_map.end() ? kBadId : itr->second.id;
}

oid_t CtpTrader::id_of( const ex_oid_t& eoid_ ) {
    auto itr = std::find_if( _id_map.begin(), _id_map.end(), [ & ]( auto& pair_ ) {
        return eoid_ == pair_.second.eoid;
    } );

    return itr == _id_map.end() ? kBadId : itr->second.id;
}

oid_t CtpTrader::id_of( const ex_oid_t& exoid_, const TThostFtdcOrderRefType& ref_ ) {
    oid_t id = kBadId;

    if ( IS_VALID_ID( ref_ ) )
        id = id_of( ref_ );

    if ( !IS_VALID_ID( id ) && exoid_.is_valid() )
        id = id_of( exoid_ );

    return id;
}

/////////////////////////////////////////////////////////////////////////
/// 开仓TThostFtdcOffsetFlagType
// #define THOST_FTDC_OF_Open '0'
/// 平仓
// #define THOST_FTDC_OF_Close '1'
/// 强平
// #define THOST_FTDC_OF_ForceClose '2'
/// 平今
// #define THOST_FTDC_OF_CloseToday '3'
/// 平昨
// #define THOST_FTDC_OF_CloseYesterday '4'
/// 强减
// #define THOST_FTDC_OF_ForceOff '5'
/// 本地强平
// #define THOST_FTDC_OF_LocalForceClose '6'

// TThostFtdcDirectionType
/////买
// #define THOST_FTDC_D_Buy '0'
/// 卖
// #define THOST_FTDC_D_Sell '1'

// TThostFtdcCombOffsetFlagType 为多腿组合订单，单腿只用[0]
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

void CtpTrader::OnRtnOrder( CThostFtdcOrderField* f_ ) {
    LOG_INFO( "[ctp] OnOrderRtn[1]@front=%d sess=%d ref=%s exid=%s sysid=%s\n", f_->FrontID, f_->SessionID, f_->OrderRef, f_->ExchangeID, f_->OrderSysID );

    auto oid = id_of( { f_->ExchangeID, f_->OrderSysID }, f_->OrderRef ) == 0;
    if ( !IS_VALID_ID( oid ) ) {
        LOG_INFO( "cannot reg order id ex= %s syso=%d ref=%s", f_->ExchangeID, f_->OrderSysID, f_->OrderRef );
        return;
    }

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

    // 说明，单子的终止状态包括 0(全部成交)，2(部分成交)， 5(全部主动撤单，或为系统判断为废单)
    // 思考：单子的状态是单线程推送，时间先后顺序应该有保障的！
    // https://www.cnblogs.com/leijiangtao/p/5379133.html

    switch ( f_->OrderStatus ) {
    default:
    case THOST_FTDC_OST_NoTradeQueueing:     // 已经报到交易所，但是未成交，注意，如果报到交易所并且立即成交，第二个RtnOrder回调状态也是unkonwn
    case THOST_FTDC_OST_NotTouched:          // 预埋单尚未触发
    case THOST_FTDC_OST_NoTradeNotQueueing:  // 还未发往交易所是不是最终状态?
    case THOST_FTDC_OST_Touched:
    case THOST_FTDC_OST_Unknown:  // 收到保单后第一次返回，表示ctp接受订单，通过ctp的风控检查
        return _om->update( oid, ostatus_t::pending );

    case THOST_FTDC_OST_PartTradedQueueing:       // 还会有成交--我们会同步过程不能在这里结束
    case THOST_FTDC_OST_AllTraded:                // ctpman-最终状态（1）
    case THOST_FTDC_OST_PartTradedNotQueueing: {  // todo 最终状态（2）已经不在队列中--剩余部分已经撤单---订单已完成标志?---
        order_t o = { 0 };

        o.id     = oid;
        o.qty    = f_->VolumeTraded;  // o.qty准备返回结果//f->VolumeTotal，这是剩余数量
        o.remark = f_->StatusMsg;

        o.datetime.from_ctp( f_->InsertDate, f_->InsertTime, 0 );  // inserttime 是ctp本地时间或者交易所时间（返回后）
        o.code = f_->InstrumentID;

        o.status = ostatus_t::dealt;  // note! OrderSubmitStatus：CTP内部使用，普通投资者可以忽略
        LOG_INFO( "order dir %d %s", f_->Direction, f_->CombOffsetFlag );

        o.dir = cvt_direction( f_->Direction, f_->CombOffsetFlag );
        if ( odir_t::none == o.dir ) {
            LOG_INFO( "bad order direction'" );
        }

        o.price = f_->LimitPrice;

        return _om->update( o );
    } break;

    case THOST_FTDC_OST_Canceled:  // 最终状态（4），当ordersubmitstatus为thost_ftdc_accepts时是主动撤单
        return _om->update( oid, ostatus_t::cancelled );
    }
}

/*-------------------------------------------------------------------------
    如果该报单由交易所进行了撮合成交，交易所再次返回该报单的状态（已成交）。并通过此函数返回该笔成
    交。报单成交之后，一个报单回报（OnRtnOrder）和一个成交回报（OnRtnTrade）会被发送到客户端，报单回报
    中报单的状态为“已成交”。但是仍然建议客户端将成交回报作为报单成交的标志，因为 CTP 的交易核心在
    收到 OnRtnTrade 之后才会更新该报单的状态。如果客户端通过报单回报来判断报单成交与否并立即平仓，
    有极小的概率会出现在平仓指令到达 CTP 交易核心时该报单的状态仍未更新，就会导致无法平仓。
----------------------------------------------------------------------------*/
void CtpTrader::OnRtnTrade( CThostFtdcTradeField* f_ ) {
    auto id = id_of( f_->OrderRef );

    if ( !IS_VALID_ID( id ) ) {
        LOG_INFO( "rtn trade, cannot reg order id" );
        return;
    }

    order_t o = { 0 };
    o.id      = id;
    o.datetime.from_ctp( f_->TradeDate, f_->TradeTime, 0 );
    o.qty   = f_->Volume;
    o.code  = f_->InstrumentID;
    o.price = f_->Price;
    o.dir   = cvt_direction( f_->Direction, f_->OffsetFlag );

    if ( odir_t::none == o.dir ) {
        LOG_INFO( "bad order direction'" );
    }

    _om->update( id, ostatus_t::finished );
}

void CtpTrader::OnErrRtnOrderInsert( CThostFtdcInputOrderField* f_, CThostFtdcRspInfoField* pRspInfo ) {
    LOG_INFO( "INSERT ERROR,errid=%d msg=%s", pRspInfo->ErrorID, pRspInfo->ErrorMsg );

    auto id = id_of( f_->OrderRef );

    if ( !IS_VALID_ID( id ) ) {
        LOG_INFO( "errrtn insert, cannot reg order id" );
        return;
    }

    _om->update( id, ostatus_t::error );
}

// 删除失败对现有的订单没有任何影响(no update)，但是要通知算法
void CtpTrader::OnErrRtnOrderAction( CThostFtdcOrderActionField* pOrderAction, CThostFtdcRspInfoField* pRspInfo ) {
    LOG_INFO( "order action error: {frontid=%d ,sessionid=%d, oref=%d} ,{ex=%s sysid=%s} , instument=%s, msg=%s, req=%d",
              pOrderAction->FrontID,
              pOrderAction->SessionID,
              pOrderAction->OrderRef,
              pOrderAction->ExchangeID,
              pOrderAction->OrderSysID,
              pOrderAction->InstrumentID,
              pRspInfo->ErrorID,
              pRspInfo->ErrorMsg );
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

void CtpTrader::OnRtnInstrumentStatus( CThostFtdcInstrumentStatusField* pInstrumentStatus ) {
}

void CtpTrader::OnRspQryInstrumentMarginRate( CThostFtdcInstrumentMarginRateField* pInstrumentMarginRate, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpTrader::OnRspQryExchangeMarginRateAdjust( CThostFtdcExchangeMarginRateAdjustField* pExchangeMarginRateAdjust, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpTrader::OnRspQryInstrumentCommissionRate( CThostFtdcInstrumentCommissionRateField* pInstrumentCommissionRate, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}
}  // namespace ctp
CUB_NS_END