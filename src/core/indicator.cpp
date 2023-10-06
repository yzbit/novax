#include "indicator.h"

#include "algo_repo.h"
#include "aspect.h"
#include "log.hpp"
#include "series.h"

CUB_NS_BEGIN

Indicator* Indicator::create( const string_t& name_, const arg_pack_t& args_, Aspect* asp_ ) {
    if ( ALGO.find( name_ ) == ALGO.end() ) {
        LOG_INFO( "cant not find indicator for %s", name_.c_str() );
    }

    Indicator* i = ALGO[ name_ ]( args_ );

    if ( i ) {
        i->set_asp( asp_ );
        asp_->attach( i );
    }

    return i;
}

int Indicator::prio() {
    return _prio;
}

void Indicator::set_prio( int p_ ) {
    _prio = p_;
}

Series* Indicator::add_series( int track_, int size_ ) {
    if ( _series.find( track_ ) != _series.end() ) {
        LOG_INFO( "series of track %d already EXISTING", track_ );
    }

    _series.emplace( track_, new Series( size_ ) );

    return _series[ track_ ];
}

Series* Indicator::track( int index_ ) {
    return _series.find( index_ ) == _series.end()
               ? nullptr
               : _series[ index_ ];
}

CUB_NS_END