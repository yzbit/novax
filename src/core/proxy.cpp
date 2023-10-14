#include "proxy.h"

#include "ctp/ctp_md_proxy.h"
#include "ctp/ctp_trade_proxy.h"

CUB_NS_BEGIN

Data::Delegator* ProxyFactory::create_data( Data* d_, int type_ ) {
    return new ctp::CtpExMd( d_ );
}

Trader::Delegator* ProxyFactory::create_trader( Trader* t_, int type_ ) {
    return new ctp::CtpTrader( t_ );
}

CUB_NS_END