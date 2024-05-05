#ifndef BB813C61_80B5_4C65_966A_8F423BC7ECEA
#define BB813C61_80B5_4C65_966A_8F423BC7ECEA
#include "definitions.h"
#include "models.h"
#include "ns.h"

NVX_NS_BEGIN

struct Canlendar {
    int  load_schedule( const char* cal_file_ );
    bool is_trade_day();
    bool is_trade_day( const datespec_t& date_ );
    bool is_trade_time( const timespec_t& time_ );
    bool is_weekend( const datetime_t& dt_ );

private:
    datespec_t previous_day( const datespec_t& date_ );
    bool       is_leap_year( int year_ );
    int        month_days( int m_ );
};

NVX_NS_END

#endif /* BB813C61_80B5_4C65_966A_8F423BC7ECEA */
