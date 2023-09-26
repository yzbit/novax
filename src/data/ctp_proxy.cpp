#include "ctp_proxy.h"

CUB_NS_BEGIN

CtpExMd::CtpExMd() {
}

int CtpExMd::subscribue( const code_t& code_ ) {

    return 0;
}

int CtpExMd::unsubscribue( const code_t& code_ ) {

    return 0;
}

int CtpExMd::init() {
    _api = CThostFtdcMdApi::CreateFtdcMdApi();
    return 0;
}

// ctp overrides
void CtpExMd::OnFrontConnected() {
}

void CtpExMd::OnFrontDisconnected( int nReason ) {
}

void CtpExMd::OnHeartBeatWarning( int nTimeLapse ) {
}

void CtpExMd::OnRspUserLogin( CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpExMd::OnRspUserLogout( CThostFtdcUserLogoutField* pUserLogout, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpExMd::OnRspQryMulticastInstrument( CThostFtdcMulticastInstrumentField* pMulticastInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpExMd::OnRspError( CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpExMd::OnRspSubMarketData( CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpExMd::OnRspUnSubMarketData( CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpExMd::OnRspSubForQuoteRsp( CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpExMd::OnRspUnSubForQuoteRsp( CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) {
}

void CtpExMd::OnRtnDepthMarketData( CThostFtdcDepthMarketDataField* pDepthMarketData ) {
}

void CtpExMd::OnRtnForQuoteRsp( CThostFtdcForQuoteRspField* pForQuoteRsp ) {
}

CUB_NS_END
