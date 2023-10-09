#include "canlendar.h"

CUB_NS_BEGIN

int Canlendar::load_schedule( const variety_t& v_ ) {
    return 0;
}

bool Canlendar::is_trading_day() {
    return true;
}

bool Canlendar::is_trading_day( const datetime_t& dt_ ) {
    return true;
}

time_range_t Canlendar::trading_time( const code_t& c_ = "" ) {

    return time_range_t();
}

Canlendar& instance() {
    static Canlendar can;
    return can;
}

CUB_NS_END
