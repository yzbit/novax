#ifndef AB88A976_581A_4E7C_A4D4_45CCCE67B257
#define AB88A976_581A_4E7C_A4D4_45CCCE67B257
#include <ctp/ThostFtdcTraderApi.h>
#include <unordered_map>
#include <vector>

#include "../definitions.h"
#include "../models.h"
#include "../ns.h"
#include "../order_mgmt.h"
#include "comm.h"

/*最近对ctp的流程有了更多的认识。

两类指令的流程:新订单和撤单操作，都是分成两大步骤，第一步是把订单交给ctp系统，他返回一个响应，
如果正确ctp会把这个订单提交给交易前端，状态通过onorderreturn和orderaction从交易前端返回

对新订单而言，最后还可能返回一个ordertraded消息

ctpmannual中将，订单的最终状态：
我们主要处理的其实就是这几个状态，其他的都是中间状态
THOST_FTDC_OST_AllTraded 、 THOST_FTDC_OST_Canceled 、 THOST_FTDC_OST_NoTradeNotQueueing 、 THOST_FTDC_OST_PartTradedNotQueueing。

从文档来看
所有的成交都会OnRtnTrade，那么对于成交状态，我们可以忽略orderreturn的处理，只监听onrtntrade

但是实际上onrtntrade的状态信息少量很多，和别人交流之后认为还是处理onorderrtn最好，把onrtntrade当成中间状态

--但是据群友说成交价格实在onrtntrade中的

==>最终还是要照顾和回归ctp的设计哲学，orderrtn只用来更新状态，rtntrade用来结束报单


最终设计的清晰思路：
下单--》状态更新（中间状态）--》结束状态用来结束等待


sessionid orderref，ordersysid等只是为了撤单用的，如果重新登陆，那么如何去查询
*/

CUB_NS_BEGIN
namespace ctp {

struct CtpTrader : OrderMgmt::Delegator, CThostFtdcTraderSpi {
    CtpTrader( OrderMgmt* om_ );

protected:
    int start() override;
    int stop() override;
    int put( const order_t& o_ ) override;
    int cancel( oid_t o_ ) override;

private:
    int login();
    int logout();
    int auth();
    int teardown();
    int qry_settlement();
    int confirm_settlement();
    int qry_fund();
    int qry_marginrate();
    int qry_commission();
    int qry_position();

private:
    int    assign_ref( oid_t id_ );
    oid_t  id_of_ref( const TThostFtdcOrderRefType& ref_ );
    odir_t cvt_direction( const TThostFtdcDirectionType& di_, const TThostFtdcCombOffsetFlagType& comb_ );
    odir_t cvt_direction( const TThostFtdcDirectionType& di_, const TThostFtdcOffsetFlagType& comb_ );

private:
    CThostFtdcTraderApi* _api;

private:
    struct session_t {
        TThostFtdcFrontIDType   front;
        TThostFtdcSessionIDType sess;
        TThostFtdcOrderRefType  init_ref;  // 因为这个字段是从右边对齐，为了处理方便，我们可以先给他一个很大的值，比如'1000000000'，这样每次只需要处理进位即可

        session_t() = default;
        session_t( TThostFtdcFrontIDType f_, TThostFtdcSessionIDType s_, const TThostFtdcOrderRefType& r_ );
    };

    struct ex_oid_t {
        TThostFtdcExchangeIDType ex;
        TThostFtdcOrderSysIDType oid;
        ex_oid_t();
        ex_oid_t( const TThostFtdcExchangeIDType& ex_, const TThostFtdcOrderSysIDType& oid_ );
    };

    struct order_ids_t {
        oid_t                  id;
        ex_oid_t               eoid;
        TThostFtdcOrderRefType ref;

        order_ids_t( oid_t id_, const ex_oid_t& ex_oid_, const TThostFtdcOrderRefType& ref_ );
        bool is_exoid_valid();
        bool is_id_valid();
        bool is_ref_valid();
    };

    void session_changed( const session_t& s_ );
    int  update_order_ids( order_ids_t& ids_ );

private:
    using pending_t = std::unordered_map<oid_t, order_ids_t>;

    setting_t _settings;
    pending_t _orders;
    session_t _ss;
    req_map_t _reqs;

private:
    OrderMgmt* _om;

private:
    void OnRtnBulletin( CThostFtdcBulletinField* pBulletin ) override;                                                                                                                     /// 交易所公告通知
    void OnRtnTradingNotice( CThostFtdcTradingNoticeInfoField* pTradingNoticeInfo ) override;                                                                                              /// 交易通知
    void OnFrontConnected() override;                                                                                                                                                      // 当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
    void OnRspAuthenticate( CThostFtdcRspAuthenticateField* pRspAuthenticateField, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;                              // 客户端认证
    void OnRspUserLogin( CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;                                            /// 登录请求响应
    void OnRspError( CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;                                                                                            /// 错误应答
    void OnFrontDisconnected( int nReason ) override;                                                                                                                                      /// 当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
    void OnHeartBeatWarning( int nTimeLapse ) override;                                                                                                                                    /// 心跳超时警告。当长时间未收到报文时，该方法被调用。
    void OnRspUserLogout( CThostFtdcUserLogoutField* pUserLogout, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;                                               /// 登出请求响应
    void OnRspSettlementInfoConfirm( CThostFtdcSettlementInfoConfirmField* pSettlementInfoConfirm, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;              /// 投资者结算结果确认响应
    void OnRspQrySettlementInfo( CThostFtdcSettlementInfoField* pSettlementInfo, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;                                //
    void OnRspQryInstrument( CThostFtdcInstrumentField* pInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;                                            /// 请求查询合约响应
    void OnRspQryTradingAccount( CThostFtdcTradingAccountField* pTradingAccount, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;                                /// 请求查询资金账户响应
    void OnRspQryOrder( CThostFtdcOrderField* pOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;                                                           /// 请求查询报单响应
    void OnRspQryTrade( CThostFtdcTradeField* pTrade, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;                                                           /// 请求查询成交响应
    void OnRspQryInvestorPositionDetail( CThostFtdcInvestorPositionDetailField* pInvestorPositionDetail, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;        //
    void OnRspQryInvestorPosition( CThostFtdcInvestorPositionField* pInvestorPosition, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;                          /// 请求查询投资者持仓响应
    void OnRspOrderInsert( CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;                                              /// 报单录入请求响应
    void OnRspOrderAction( CThostFtdcInputOrderActionField* pInputOrderAction, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;                                  /// 报单操作请求响应
    void OnRtnOrder( CThostFtdcOrderField* pOrder ) override;                                                                                                                              /// 报单通知
    void OnErrRtnOrderInsert( CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo ) override;                                                                         //
    void OnErrRtnOrderAction( CThostFtdcOrderActionField* pOrderAction, CThostFtdcRspInfoField* pRspInfo ) override;                                                                       //
    void OnRtnTrade( CThostFtdcTradeField* pTrade ) override;                                                                                                                              /// 成交通知
    void OnRtnInstrumentStatus( CThostFtdcInstrumentStatusField* pInstrumentStatus ) override;                                                                                             // 交易所发生变化的时候会广播这个消息
    void OnRspQryInstrumentMarginRate( CThostFtdcInstrumentMarginRateField* pInstrumentMarginRate, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;              // 保证金率
    void OnRspQryExchangeMarginRateAdjust( CThostFtdcExchangeMarginRateAdjustField* pExchangeMarginRateAdjust, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;  //
    void OnRspQryInstrumentCommissionRate( CThostFtdcInstrumentCommissionRateField* pInstrumentCommissionRate, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;  // 手续费
};

inline CtpTrader::session_t::session_t( TThostFtdcFrontIDType f_, TThostFtdcSessionIDType s_, const TThostFtdcOrderRefType& r_ )
    : front( f_ )
    , sess( s_ ) {
    memcpy( init_ref, r_, sizeof( init_ref ) );
}

inline CtpTrader::ex_oid_t::ex_oid_t() {
    memset( ex, 0x00, sizeof( ex ) );
    memset( oid, 0x00, sizeof( oid ) );
}

inline CtpTrader::ex_oid_t::ex_oid_t( const TThostFtdcExchangeIDType& ex_, const TThostFtdcOrderSysIDType& oid_ ) {
    memcpy( ex, ex_, sizeof( ex ) );
    memcpy( oid, oid_, sizeof( oid ) );
}

inline CtpTrader::order_ids_t::order_ids_t( oid_t id_, const ex_oid_t& ex_oid_, const TThostFtdcOrderRefType& ref_ ) {
    id = id_;
    memcpy( &eoid, &ex_oid_, sizeof( eoid ) );
    memcpy( ref, ref_, sizeof( ref ) );
}

inline bool CtpTrader::order_ids_t::is_exoid_valid() {
    return eoid.ex[ 0 ] != '\0';
}

inline bool CtpTrader::order_ids_t::is_id_valid() {
    return IS_VALID_ID( id );
}

inline bool CtpTrader::order_ids_t::is_ref_valid() {
    return ref[ sizeof ref - 1 ] != '\0';  // 靠右对齐
}

}  // namespace ctp

CUB_NS_END
#endif /* AB88A976_581A_4E7C_A4D4_45CCCE67B257 */
