#include "aspect.h"

#include "data.h"
#include "indicator.h"
#include "kline.h"
#include "log.hpp"

CUB_NS_BEGIN

Aspect::Aspect( const code_t& code_, const period_t& p_, int count_ )
    : _symbol( code_ ) {

    load( code_, p_, count_ );
}

Aspect::~Aspect() {
    delete _k;

    for ( auto& i : _algos ) {
        delete i;
    }

    _algos.clear();
}

int Aspect::load( const code_t& code_, const period_t& p_, int count_ ) {
    if ( DATA.subscribe( code_ ) < 0 ) {
        LOG_INFO( "subs code failed : %s", code_ );
        return -1;
    }

    _k = Kline::create( { code_, p_, count_ } );
    CUB_ASSERT( _k );

    return 0;
}

void Aspect::update( const quotation_t& q_ ) {
    _k->on_calc( q_ );

    for ( auto& i : _algos ) {
        i->on_calc( q_ );
    }
}

int Aspect::attach( Indicator* i_ ) {
    if ( _algos.end() == std::find( _algos.begin(), _algos.end(), i_ ) )
        _algos.push_back( i_ );

    _algos.sort( []( auto& a_, auto& b_ ) { return a_->prio() > b_->prio(); } );

    return 0;
}

Indicator* Aspect::attach( const string_t& name_, const arg_pack_t& args_ ) {
    auto i = Indicator::create( name_, args_, this );

    if ( !i ) return nullptr;
    i->on_init();

    attach( i );

    return i;
}

CUB_NS_END
