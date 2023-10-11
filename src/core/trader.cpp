
#include "trader.h"

#include "proxy.h"

CUB_NS_BEGIN

Trader& Trader::instance() {
    static Trader* t = ProxyFactory::create_trader( 0 );
    return *t;
}

CUB_NS_END