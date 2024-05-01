#ifndef AB88A976_581A_4E7C_A4D4_45CCCE67B257
#define AB88A976_581A_4E7C_A4D4_45CCCE67B257
#include <ctp/ThostFtdcTraderApi.h>
#include <unordered_map>
#include <vector>

#include "../definitions.h"
#include "../models.h"
#include "../ns.h"
#include "../order_mgmt.h"
#include "../proxy.h"
#include "comm.h"

NVX_NS_BEGIN

#define CTP_TRADE_SETTING_FILE "/home/data/code/cub/src/core/ctp/ctp_trade.json"

#define IS_VALID_REF( _ref_ ) ( _ref_[ sizeof( _ref_ ) - 1 ] != '\0' )
#define IS_EQUAL_REF( _a_, _b_ ) ( 0 == memcmp( _a_, _b_, sizeof _a_ ) )

namespace ctp {

struct CtpTrader : IBroker, CThostFtdcTraderSpi {
    CtpTrader( ITrader* tr_ );

protected:
    int start() override;
    int stop() override;
    int put( const order_t& o_ ) override;
    int cancel( const order_t& o_ ) override;

private:
    int login();
    int logout();
    int auth();
    int qry_settlement();
    int confirm_settlement();
    int qry_fund();
    int qry_marginrate();
    int qry_commission();
    int qry_position();

private:
    int    assign_ref( oid_t id_ );
    oid_t  id_of( const TThostFtdcOrderRefType& ref_ );
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

        bool is_valid() const;
        bool operator==( const ex_oid_t& eoid_ ) const;
    };
    struct order_ids_t {
        oid_t                  id;
        ex_oid_t               eoid;
        TThostFtdcOrderRefType ref;

        void set_ref( const TThostFtdcOrderRefType& ref_ );
        order_ids_t() = default;
        order_ids_t( oid_t id_, const ex_oid_t& ex_oid_, const TThostFtdcOrderRefType& ref_ );
    };

    oid_t id_of( const ex_oid_t& eoid_ );
    void  session_changed( const session_t& s_ );
    oid_t id_of( const ex_oid_t& exoid_, const TThostFtdcOrderRefType& ref_ );

private:
    using IdMap = std::unordered_map<oid_t, order_ids_t>;

    setting_t _settings;
    IdMap     _id_map;
    session_t _ss;

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

inline bool CtpTrader::ex_oid_t::is_valid() const {
    return ex[ 0 ] != '\0' && oid[ 0 ] != '\0';
}

inline CtpTrader::ex_oid_t::ex_oid_t( const TThostFtdcExchangeIDType& ex_, const TThostFtdcOrderSysIDType& oid_ ) {
    memcpy( ex, ex_, sizeof( ex ) );
    memcpy( oid, oid_, sizeof( oid ) );
}

inline bool CtpTrader::ex_oid_t::operator==( const ex_oid_t& eoid_ ) const {
    return 0 == memcmp( eoid_.oid, oid, sizeof( eoid_.oid ) )
           && 0 == memcmp( eoid_.ex, ex, sizeof( ex ) );
}

inline CtpTrader::order_ids_t::order_ids_t( oid_t id_, const ex_oid_t& ex_oid_, const TThostFtdcOrderRefType& ref_ ) {
    id = id_;
    memcpy( &eoid, &ex_oid_, sizeof( eoid ) );
    memcpy( ref, ref_, sizeof( ref ) );
}

inline void CtpTrader::order_ids_t::set_ref( const TThostFtdcOrderRefType& ref_ ) {
    memcpy( ref, ref_, sizeof( ref ) );
}
}  // namespace ctp

NVX_NS_END
#endif /* AB88A976_581A_4E7C_A4D4_45CCCE67B257 */
