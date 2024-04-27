#include "canlendar.h"

NVX_NS_BEGIN

int Canlendar::load_schedule( const variety_t& v_ ) {
    return 0;
}

bool Canlendar::is_trade_day() {
    return true;
}

bool Canlendar::is_trade_day( const datetime_t& dt_ ) {
    return true;
}

time_range_t Canlendar::trade_time( const code_t& c_ = "" ) {

    return time_range_t();
}


Canlendar& instance() {
    static Canlendar can;
    return can;
}

NVX_NS_END
