#include <list>

#include "context.h"

#include "data.h"
#include "log.hpp"

CUB_NS_BEGIN

std::list<Aspect*> _aspects;

Aspect* Context::aspect( const code_t& symbol_, const period_t& period_, int count_ ) {
    if ( DATA.subcribe( symbol_ ) < 0 ) {
        return nullptr;
    }

    Aspect* a = new Aspect( symbol_, period_, count_ ) {}

    _aspects.push_back( a );
    return a;
}

int Context::load( const string_t& algo_name_, const ArgPack& arg_ ) {
    return 0;
}

int Context::load( Algo* a_ ) {
    assert( a_ ) {}
    _algo = a_;
    return 0;
}

//------市价下单------
int Context::pshort( const code_t& c_, vol_t qty_ ) {
    // todo ask?
    return pshort( { c_, qty_, q().ask[ 0 ], mode_t::market } );
}

int Context::plong( const code_t& c_, vol_t qty_ ) {
    return plong( { c_, qty_, q().bid[ 0 ], mode_t::market } );
}

int Context::cshort( const code_t& c_, vol_t qty_ ) {
    return cshort( { c_, qty_, q().bid[ 0 ], mode_t::market } );
}

int Context::clong( const code_t& c_, vol_t qty_ ) {
    return clong( { c_, qty_, q().ask[ 0 ], mode_t::market } );
}

//------高级下单模式-----
int Context::pshort( const attr_t& a_ ) {
    return OMGMT.sellshort( a_, 0, 0 );
}

int Context::plong( const attr_t& a_ ) {
    return OMGMT.buylong( a_, 0, 0 );
}

int Context::cshort( const attr_t& a_ ) {
    return OMGMT.cover( a_ );
}

int Context::clong( const attr_t& a_ ) {
    return OMGMT.sell( a_ );
}

//-------仓位查询-------
Portfolio& Context::p() {
    return OMGMT.portfolio();
}

vol_t Context::position() {  //已成交持仓
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

vol_t Context::pending() {  //未成交持仓
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
    retturn 0;
}

int Context::last_entry() {  //最近入场的k线
    return 0;
}

int Context::last_exit() {  //最近出场的k线//和aspect相关
    return 0;
}

CUB_NS_END