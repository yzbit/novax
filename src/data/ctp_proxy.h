#ifndef DF548D9F_3819_44FA_89F5_6218193C54F2
#define DF548D9F_3819_44FA_89F5_6218193C54F2
#include <comm/clock.h>
#include <comm/definitions.h>
#include <comm/utils.h>
#include <ctp/ThostFtdcMdApi.h>
#include <cub_ns.h>
#include <mutex>
#include <set>

#include "market.h"

#define CTP_MD_SETTING_FILE "ctp_md.json"

CUB_NS_BEGIN
struct CtpExMd : Market, CThostFtdcMdSpi {

    CtpExMd();

    int subscribue( const code_t& code_ ) override;
    int unsubscribue( const code_t& code_ ) override;

private:
    int init() override;
    int login();

private:
    struct conn_t {
        string_t broker;
        string_t frontend;
        string_t user_name;
        string_t password;
    };

    struct setting_t {
        string_t flow_path;
        conn_t   conn;
    };

    struct cert_t {
        string_t auth;
        string_t appid;
        string_t token;
    };

    enum class extype_t : uint8_t {
        SHFE = 0,
        DCE,
        CZCE,
        FFEX,
        INE,
        GFEX,

        Count
    };

private:
    int read_settings();
    int sub();
    int unsub();
    int sub( code_t& code_ );
    int unsub( code_t& code_ );

private:
    std::unique_ptr<char*[]> set2arr( std::set<code_t>& s );

private:
    void cvt_datetime( DateTime&                     dt,
                       const TThostFtdcDateType&     ctp_day_,
                       const TThostFtdcTimeType&     ctp_time_,
                       const TThostFtdcMillisecType& ctp_milli_ );

private:
    // ctp overrides
    void OnFrontConnected() override;
    void OnFrontDisconnected( int nReason ) override;
    void OnHeartBeatWarning( int nTimeLapse ) override;
    void OnRspUserLogin( CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;
    void OnRspUserLogout( CThostFtdcUserLogoutField* pUserLogout, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;
    void OnRspQryMulticastInstrument( CThostFtdcMulticastInstrumentField* pMulticastInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;
    void OnRspError( CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;
    void OnRspSubMarketData( CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;
    void OnRspUnSubMarketData( CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;
    void OnRspSubForQuoteRsp( CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;
    void OnRspUnSubForQuoteRsp( CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;
    void OnRtnDepthMarketData( CThostFtdcDepthMarketDataField* pDepthMarketData ) override;
    void OnRtnForQuoteRsp( CThostFtdcForQuoteRspField* pForQuoteRsp ) override;

private:
    CThostFtdcMdApi* _api = nullptr;
    setting_t        _settings;
    std::set<code_t> _sub_symbols;
    std::set<code_t> _unsub_symbols;
    bool             _is_svc_online = false;
    Clock            _clock[ ( int )( extype_t::Count ) ];
    std::mutex       _sub_mtx;

private:
    static id_t session_id();
};
CUB_NS_END

#endif /* DF548D9F_3819_44FA_89F5_6218193C54F2 */
