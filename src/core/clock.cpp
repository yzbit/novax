#include <unordered_map>

#include "clock.h"

#include "log.hpp"

CUB_NS_BEGIN

Clock::Clock()
    : _epoch( time( 0 ) )
    , _drift( 0 ) {}

void Clock::tune( const datetime_t& dt_ ) {
    _epoch = time( 0 );
    _drift = dt_.to_unix_time() - _epoch;

    LOG_TAGGED( "clock", "ex clock drift %u", _drift );
}

time_t Clock::now() {
    return time( 0 ) + _drift;
}

Clock& Clock::of_exchange( int exid_ ) {
    static std::unordered_map<int, Clock> _c_repo;

    auto [ p, b ] = _c_repo.try_emplace( exid_, Clock() );
    return p->second;
}

CUB_NS_END