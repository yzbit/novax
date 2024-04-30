#include <cassert>
#include <list>

#include "aspect.h"
#include "data.h"
#include "log.hpp"
#include "order_mgmt.h"
#include "quant.h"

NVX_NS_BEGIN

Context::Context( Quant* q_ )
    : _q( q_ ) {}

quotation_t& Context::qut() {
    return _qut;
}

fund_t& Context::fund() {
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

Context* Context::create() {
    static Context c;
    return &c;
}

Aspect* QuantImpl::add_aspect( const code_t& symbol_, const period_t& period_, int count_ ) {
    return _d->attach( symbol_, period_, count_ );
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
        return TRADER.closeall( c_ );
    }
}
vol_t Context::position() {  // 已成交持仓
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

vol_t Context::position( const code_t& c_ ) {
    return 0;
}

vol_t Context::pending() {  // 未成交持仓
}

vol_t Context::pending( const code_t& c_ ) {}

price_t Context::put_price() {
    return 0;
}

price_t Context::last_deal() {
}

int Context::last_entry() {  // 最近入场的k线
    return 0;
}

int Context::last_exit() {  // 最近出场的k线//和aspect相关
    return 0;
}

NVX_NS_END