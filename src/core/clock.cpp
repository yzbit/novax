#include "clock.h"

CUB_NS_BEGIN

void Clock::tune( const datetime_t& dt_ ) {
    // 把当前的时间点， timepoint调整为 dt_，并且记录下
    struct tm t  = dt_.tm();
    auto      tt = mktime( &t );

    epoch = time( 0 );

    diff = tt - epoch;
}

time_t Clock::now() {
    return time( 0 ) + diff;
}

CUB_NS_END