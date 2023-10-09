
#include "trader.h"

#include "ctp/ctp_trade_proxy.h"

CUB_NS_BEGIN

Trader& Trader::instance() {
    static ctp::CtpTrader _ctp;
    return _ctp;
}

CUB_NS_END