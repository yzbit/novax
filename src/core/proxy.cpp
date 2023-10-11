#include "proxy.h"

#include "ctp/ctp_md_proxy.h"
#include "ctp/ctp_trade_proxy.h"

CUB_NS_BEGIN

Data* ProxyFactory::create_data( int type_ ) {
    return new ctp::CtpExMd();
}

Trader* ProxyFactory::create_trader( int type_ ) {
    return new ctp::CtpTrader();
}

CUB_NS_END