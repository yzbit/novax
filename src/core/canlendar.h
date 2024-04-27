#ifndef BB813C61_80B5_4C65_966A_8F423BC7ECEA
#define BB813C61_80B5_4C65_966A_8F423BC7ECEA
#include "definitions.h"
#include "models.h"
#include "ns.h"

NVX_NS_BEGIN

struct Canlendar {
    static Canlendar& instance();

    int          load_schedule( const variety_t& v_ );
    bool         is_trade_day();
    bool         is_trade_day( const datetime_t& dt_ );
    time_range_t trade_time( const code_t& c_ );
};

NVX_NS_END

#define CANLEN Canlendar::instance()

#endif /* BB813C61_80B5_4C65_966A_8F423BC7ECEA */
