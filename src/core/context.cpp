#include <list>

#include "aspect.h"
#include "data.h"
#include "log.hpp"
#include "order_mgmt.h"
#include "quant_impl.h"

CUB_NS_BEGIN

QuantImpl::QuantImpl( QuantImpl* q_ )
    : _dctx( q_ )
    , _octx( q_ ) {
    _def_asp = new Aspect();
    _q->data()->attach( _def_asp );
}

std::list<Aspect*> _aspects;

Aspect* QuantImpl::aspect() {
    return _def_asp;
}

Context* Context::create( QuantImpl* q_ ) {
    return q_;
}

Aspect* QuantImpl::add_aspect( const code_t& symbol_, const period_t& period_, int count_ ) {
    return _q->data()->attach( symbol_, period_, count_ );
}

int QuantImpl::pshort( const code_t& c_, vol_t qty_, price_t price_, otype_t mode_ ) {
    return _q->mgmt()->sellshort( { c_, qty_, price_, mode_ } );
}

int QuantImpl::plong( const code_t& c_, vol_t qty_, price_t price_, otype_t mode_ ) {
    return _q->mgmt()->buylong( { c_, qty_, price_, mode_ } );
}

int QuantImpl::cshort( const code_t& c_, vol_t qty_, price_t price_, otype_t mode_ ) {
    return _q->mgmt()->buy( { c_, qty_, price_, mode_ } );
}

int QuantImpl::clong( const code_t& c_, vol_t qty_, price_t price_, otype_t mode_ ) {
    return _q->mgmt()->sell( { c_, qty_, price_, mode_ } );
}

vol_t Context::position() {  // 已成交持仓
#if 0
    auto& pf = p();
    vol_t v  = 0;
    pf.for_each( []( position_t& p ) {
        v += p.position;
    } );

    return v;
#endif
    return p().dealt();
}

vol_t Context::position( const code_t& c_ ) {
    0;
}

vol_t Context::pending() {  // 未成交持仓
}

vol_t Context::pending( const code_t& c_ ) {}

Kline& Context::kline() {
    return aspect()->kline();
}

candle_t& Context::bar( int index_ ) {
    static candle_t c;
    return c;
}

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

CUB_NS_END