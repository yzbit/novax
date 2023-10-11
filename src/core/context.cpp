#include <list>

#include "context.h"

#include "aspect.h"
#include "data.h"
#include "log.hpp"
#include "order_mgmt.h"

CUB_NS_BEGIN

std::list<Aspect*> _aspects;

Aspect* Context::aspect( const code_t& symbol_, const period_t& period_, int count_ ) {
    Aspect* a = new Aspect( symbol_, period_, count_ ) {}

    _aspects.push_back( a );
    return a;
}

int Context::load( const string_t& strategy_name_, const arg_pack_t& arg_ ) {
    CUB_ASSERT( 0 );  // todo
    return 0;
}

int Context::load( std::unique_ptr<Strategy> a_ ) {
    BIND_STRATEGY( a_ );
}

//------市价下单------
int Context::pshort( const code_t& c_, vol_t qty_, price_t price_, otype_t mode_ ) {
}

int Context::plong( const code_t& c_, vol_t qty_, price_t price_, otype_t mode_ ) {
}

int Context::cshort( const code_t& c_, vol_t qty_, price_t price_, otype_t mode_ ) {
}

int Context::clong( const code_t& c_, vol_t qty_, price_t price_, otype_t mode_ ) {
}

int Context::close( const code_t& c_, vol_t qty_, price_t price_, otype_t mode_ ) {
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
}

int Context::last_entry() {  //最近入场的k线
    return 0;
}

int Context::last_exit() {  //最近出场的k线//和aspect相关
    return 0;
}

CUB_NS_END