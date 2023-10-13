#include "proxy.h"

#include "ctp/ctp_md_proxy.h"
#include "ctp/ctp_trade_proxy.h"

CUB_NS_BEGIN

Data::Delegator* ProxyFactory::create_data( Data* d_, int type_ ) {
    return new ctp::CtpExMd( d_ );
}

Trader* ProxyFactory::create_trader( int type_ ) {
    return new ctp::CtpTrader();
}

CUB_NS_END