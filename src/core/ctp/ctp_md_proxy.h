#ifndef DF548D9F_3819_44FA_89F5_6218193C54F2
#define DF548D9F_3819_44FA_89F5_6218193C54F2
#include <ctp/ThostFtdcMdApi.h>
#include <mutex>
#include <set>

#include "../clock.h"
#include "../data.h"
#include "../definitions.h"
#include "../ns.h"
#include "comm.h"

//#define CTP_MD_SETTING_FILE "ctp_md.json"
//#define CTP_MD_SETTING_FILE "/home/ubuntu/code/cub/src/core/ctp/ctp_md.json"
#define CTP_MD_SETTING_FILE "conf/ctp_md.json"
CUB_NS_BEGIN

namespace ctp {
struct CtpExMd : Data::Delegator, CThostFtdcMdSpi {
    CtpExMd( Data* d_ );

protected:
    int start() override;
    int stop() override;
    int subscribe( const code_t& code_ ) override;
    int unsubscribe( const code_t& code_ ) override;

private:
    int init();
    int login();
    int sub();
    int unsub();
    int sub( code_t& code_ );
    int unsub( code_t& code_ );

private:
    std::unique_ptr<char*[]> set2arr( std::set<code_t>& s );

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
    bool _running = false;

private:
    Data* _d;

private:
    static id_t session_id();
};
}  // namespace ctp
CUB_NS_END

#endif /* DF548D9F_3819_44FA_89F5_6218193C54F2 */
