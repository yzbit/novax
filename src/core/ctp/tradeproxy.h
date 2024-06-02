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

#ifndef AB88A976_581A_4E7C_A4D4_45CCCE67B257
#define AB88A976_581A_4E7C_A4D4_45CCCE67B257
#include <condition_variable>
#include <ctp/ThostFtdcTraderApi.h>
#include <future>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "../definitions.h"
#include "../models.h"
#include "../ns.h"
#include "../ordermgmt.h"
#include "../proxy.h"
#include "comm.h"
#include "ctpids.h"
#include "gateway.h"

NVX_NS_BEGIN

#define CTP_TRADE_SETTING_FILE "/home/data/code/cub/src/core/ctp/ctp_trade.json"

namespace ctp {

struct trader : broker, gateway, CThostFtdcTraderSpi {
    trader( ipub* tr_, int id_start_ref_ = 0 );

protected:
    nvx_st start() override;
    nvx_st stop() override;
    nvx_st cancel( const oid& id_ ) override;
    oid    put( const code& instrument_, vol qty_, price price_, ord_type mode_, ord_dir dir_ ) override;

private:
    nvx_st login();
    nvx_st logout();
    nvx_st auth();
    nvx_st qry_settlement();
    nvx_st confirm_settlement();
    nvx_st qry_fund();
    nvx_st qry_marginrate();
    nvx_st qry_commission();
    nvx_st qry_position();

private:
    void on_init() override;
    void on_release() override;

private:
    ord_dir cvt_direction( const TThostFtdcDirectionType& di_, const TThostFtdcCombOffsetFlagType& comb_ );
    ord_dir cvt_direction( const TThostFtdcDirectionType& di_, const TThostFtdcOffsetFlagType& comb_ );

private:
    CThostFtdcTraderApi* _api;

private:
    void reconnected( const session& s_, const ordref& max_ref_ );
    bool settled() { return _settled; }
    void daemon();

private:
    setting _settings;
    session _ss;
    ordref  _last_ref;
    id_mgr  _id;
    bool    _settled = false;  // 是否是必须查询settlement之后才可以下单?--感觉应该是不必要的

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

}  // namespace ctp

NVX_NS_END
#endif /* AB88A976_581A_4E7C_A4D4_45CCCE67B257 */
