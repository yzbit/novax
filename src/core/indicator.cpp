#include "indicator.h"

#include "algos/repo.h"
#include "aspect.h"
#include "log.hpp"
#include "series.h"

CUB_NS_BEGIN

std::atomic<int> Indicator::_global_prio = 1;

Indicator::Indicator() {
    _prio = ++_global_prio;
}
Indicator* Indicator::create( const string_t& name_, const arg_pack_t& args_, Aspect* asp_ ) {

    if ( ALGO.find( name_ ) == ALGO.end() ) {
        LOG_INFO( "cant not find indicator for %s", name_.c_str() );
        return nullptr;
    }

    try {
        Indicator* i = ALGO[ name_ ]( args_ );

        if ( i ) {
            i->set_asp( asp_ );
            asp_->attach( i );
        }

        return i;
    }
    catch ( ... ) {
        return nullptr;  // bad func call
    }
}

Indicator::~Indicator() {
    for ( auto& [ t, s ] : _series ) {
        delete s;
    }

    _series.clear();
}

int Indicator::prio() {
    return _prio;
}

Series* Indicator::add_series( int track_, int size_, Series::free_t free_ ) {
    if ( _series.find( track_ ) != _series.end() ) {
        LOG_INFO( "series of track %d already EXISTING", track_ );
    }

    _series.emplace( track_, new Series( size_ ) );

    return _series[ track_ ];
}

Series::element_t& Indicator::value( int track_, int index_ ) {
    return track( track_ )->at( index_ );
}

void Indicator::shift() {
    for ( auto& [ i, s ] : _series ) {
        s->shift();
    }
}

Series::element_t& Indicator::recent() {
    return value();
}

Series* Indicator::track( int index_ ) {
    return _series.find( index_ ) == _series.end()
               ? nullptr
               : _series[ index_ ];
}

CUB_NS_END