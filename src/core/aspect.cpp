#include "aspect.h"

#include "indicator.h"

CUB_NS_BEGIN

Aspect::~Aspect() {
    for ( auto& i : _algos ) {
        delete i;
    }

    _algos.clear();
}

void Aspect::update( const quotation_t& q_ ) {
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
