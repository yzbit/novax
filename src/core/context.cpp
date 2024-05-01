#include <cassert>
#include <list>

#include "context.h"

#include "aspect.h"
#include "data.h"
#include "log.hpp"
#include "order_mgmt.h"
#include "quant.h"
#include "strategy.h"

NVX_NS_BEGIN

#define TRADER ( *dynamic_cast<OrderMgmt*>( this->_q->trader() ) )

Context::Context( Quant* q_ )
    : _q( q_ ) {}

const quotation_t& Context::qut() const {
    return _qut;
}

const fund_t Context::fund() const {
    return _fund;
}

void Context::update_qut( const quotation_t& q_ ) {
    _qut = q_;
    _q->strategy()->prefight( this );
}

void Context::update_fund( const fund_t& f_ ) {
    _fund = f_;
}

Clock& Context::clock() {
    return *( _q->clock() );
}

Aspect* Context::load( const code_t& symbol_, const period_t& period_, int count_ ) {
    return ASP.add( symbol_, period_, count_ );
}

int Context::open( const code_t& c_, vol_t qty_, price_t sl_, price_t tp_, price_t price_, otype_t mode_ ) {
    if ( qty_ < 0 )
        return TRADER.sellshort( { c_, -qty_, price_, mode_ } );
    else if ( qty_ > 0 )
        TRADER.buylong( { c_, qty_, price_, mode_ } );
    else {
        assert( false );
        return 0;
    };
}

int Context::close( const code_t& c_, vol_t qty_, price_t price_, otype_t mode_ ) {
    if ( qty_ < 0 ) {
        return TRADER.buy( { c_, -qty_, price_, mode_ } );
    }
    else if ( qty_ > 0 ) {
        return TRADER.sell( { c_, qty_, price_, mode_ } );
    }
    else {
        return TRADER.close( c_ );
    }
}
vol_t Context::position() const {
#if 0
    auto& pf = p();
    vol_t v  = 0;
    pf.for_each( []( position_t& p ) {
        v += p.position;
    } );

    return v;
    return p().dealt();

#endif
    return vol_t();
}

vol_t Context::position( const code_t& c_ ) const {
    return 0;
}

vol_t Context::pending() const {
    return 0;
}

vol_t Context::pending( const code_t& c_ ) const {
    return 0;
}

price_t Context::put_price() const {
    return 0;
}

price_t Context::last_deal() const {
    return 0;
}

kidx_t Context::last_entry() const {
    return 0;
}

kidx_t Context::last_exit() const {
    return 0;
}

NVX_NS_END