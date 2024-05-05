#include "canlendar.h"

NVX_NS_BEGIN

bool Canlendar::is_trade_day() {
    return true;
}

int Canlendar::load_schedule( const char* cal_file_ ) {
    return 0;
}

bool is_trade_day() {
    return false;
}

bool is_trade_day( const datespec_t& date_ ) {
    return false;
}

bool is_trade_time( const timespec_t& time_ ) {
    return false;
}

bool is_weekend( const datetime_t& dt_ ) {
    return false;
}

NVX_NS_END
