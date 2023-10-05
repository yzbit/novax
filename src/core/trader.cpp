
#include "trader.h"

#include "ctp/ctp_trade_proxy.h"

CUB_NS_BEGIN

Trader* Trader::create() {
    return new ctp::CtpTrader();
}

CUB_NS_END