
#include "trader.h"

#include "proxy.h"

CUB_NS_BEGIN

int Trader::start() {
    return _d->start();
}

int Trader::stop() {
    return _d->stop();
}

Trader::~Trader() {
    delete _d;
}

Trader::Trader( QuantImpl* q_ )
    : _q( q_ ) {
    _d = ProxyFactory::create_trader( this, 0 );
}

int Trader::put( const order_t& o_ ) {
    return _d->put( o_ );
}

int Trader::cancel( oid_t o_ ) {
    return _d->cancel( o_ );
}

CUB_NS_END