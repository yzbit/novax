#include "indicator.h"

#include "algo_repo.h"
#include "log.hpp"

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

CUB_NS_END