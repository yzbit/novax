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

#ifndef EADD9688_434E_4982_8863_E3EEF6EA595D
#define EADD9688_434E_4982_8863_E3EEF6EA595D

#ifndef AB88A976_581A_4E7C_A4D4_45CCCE67B257
#define AB88A976_581A_4E7C_A4D4_45CCCE67B257
#include <ctp/ThostFtdcTraderApi.h>
#include <unordered_map>
#include <vector>

#include "../definitions.h"
#include "../models.h"
#include "../ns.h"
#include "../ordermgmt.h"
#include "../proxy.h"
#include "comm.h"
#include "ctpids.h"

NVX_NS_BEGIN

#define CTP_TRADE_SETTING_FILE "/home/data/code/cub/src/core/ctp/ctp_trade.json"

namespace ctp {

struct trader : broker, CThostFtdcTraderSpi {
    trader( pub* tr_, int id_start_ref_ );

protected:
    nvx_st start() override;
    nvx_st stop() override;

private:
    nvx_st login();
    nvx_st logout();
    nvx_st auth();
    nvx_st qry_settlement();
    nvx_st confirm_settlement();
    nxt_st qry_instruments();

private:
    CThostFtdcTraderApi* _api;

private:
    void reconnected( const session& s_, const ordref& max_ref_ );
    bool settled() { return _settled; }

private:
    void OnFrontConnected() override;                                                                                                                                          // 当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
    void OnRspAuthenticate( CThostFtdcRspAuthenticateField* pRspAuthenticateField, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;                  // 客户端认证
    void OnRspUserLogin( CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;                                /// 登录请求响应
    void OnRspError( CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;                                                                                /// 错误应答
    void OnFrontDisconnected( int nReason ) override;                                                                                                                          /// 当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
    void OnRspUserLogout( CThostFtdcUserLogoutField* pUserLogout, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;                                   /// 登出请求响应
    void OnRspSettlementInfoConfirm( CThostFtdcSettlementInfoConfirmField* pSettlementInfoConfirm, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;  /// 投资者结算结果确认响应
    void OnRspQrySettlementInfo( CThostFtdcSettlementInfoField* pSettlementInfo, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;                    //
    void OnRspQryInstrument( CThostFtdcInstrumentField* pInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast ) override;                                /// 请求查询合约响应
};

}  // namespace ctp

NVX_NS_END
#endif /* AB88A976_581A_4E7C_A4D4_45CCCE67B257 */

#endif /* EADD9688_434E_4982_8863_E3EEF6EA595D */
