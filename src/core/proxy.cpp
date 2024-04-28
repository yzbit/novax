#include "proxy.h"

#include "ctp/ctp_md_proxy.h"
#include "ctp/ctp_trade_proxy.h"

NVX_NS_BEGIN

ITrader::~ITrader() {}

IBroker::IBroker( ITrader* tr_ )
    : _tr( tr_ ){};

IBroker::~IBroker() {}

ITrader* IBroker::delegator() { return _tr; }

IData::~IData() {}

IMarket::IMarket( IData* dt_ )
    : _dt( dt_ ) {}

IMarket::~IMarket() {}

IData* IMarket::delegator() { return _dt; }

IMarket* create_market( IData* d_ ) {
    return new ctp::CtpExMd( d_ );
}

IBroker* create_broker( ITrader* t_ ) {
    return new ctp::CtpTrader( t_ );
}

NVX_NS_END