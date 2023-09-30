#ifndef CE4E9C03_2146_4A3B_8E20_5B6B97897D11
#define CE4E9C03_2146_4A3B_8E20_5B6B97897D11
#include <algorithm>
#include <cub_ns.h>
#include <math.h>

#include "definitions.h"

CUB_NS_BEGIN

struct candle_t {
    id_t       id;
    price_t    high;
    price_t    low;
    price_t    close;
    price_t    open;
    vol_t      volume;
    vol_t      opi;  // opi
    code_t     symbol;
    datetime_t time;

    price_t body() { return fabs( close - open ); }
    price_t long_shadow() { return std::max( uppers(), lowers() ); }
    price_t height() { return high - low; }
    price_t bar_mid() { return ( high + low ) / 2; }
    price_t body_mid() { return ( open + close ) / 2; }
    price_t price() { return close; }
    price_t uppers() { return high - body_up(); }
    price_t lowers() { return body_low() - low; }
    price_t body_up() { return std::max( close, open ); }
    price_t body_low() { return std::min( close, open ); }

    bool red() { return close > open; }
    bool white() { return red(); }
    bool black() { return close < open; }
    bool green() { return black(); }
    bool doji() { return fabs( close - open ) < 1e-4; }
    bool embrace( candle_t& o ) { return body_up() >= o.body_up() && body_low() <= o.body_low(); }
    bool close_beyond() { return close > ( high + low ) / 2; }
    bool close_under() { return close < ( high + low ) / 2; }
    bool step_beyond( const candle_t& oth_ ) { return high > oth_.high && low > oth_.low && close > oth_.close; }
    bool step_below( const candle_t& oth_ ) { return low < oth_.low && close < oth_.close && high < oth_.high; }

// k线的开始时间，主要是调试用，确保K线分割合理正确
#ifdef CUB_DEBUG
    int sday;
    int stm;
#endif
};

#define PRICE_DEPTH 5

// real time ticks
struct quotation_t {
    code_t  code;
    vol_t   volume;    // 成交仓位
    money_t turnover;  // 成交额
    vol_t   opi;

    int     depth;
    vol_t   bidvol[ PRICE_DEPTH ];
    vol_t   askvol[ PRICE_DEPTH ];
    price_t bid[ PRICE_DEPTH ];
    price_t ask[ PRICE_DEPTH ];

    price_t    highest;
    price_t    lowest;
    price_t    avgprice;    // 均价
    price_t    upperlimit;  // 涨跌停
    price_t    lowerlimit;
    price_t    last;  // 上次成交的价格
    price_t    open;
    price_t    close;
    datetime_t time;
};

CUB_NS_END
#endif /* CE4E9C03_2146_4A3B_8E20_5B6B97897D11 */
