#ifndef CE4E9C03_2146_4A3B_8E20_5B6B97897D11
#define CE4E9C03_2146_4A3B_8E20_5B6B97897D11
#include <algorithm>
#include <cub.h>
#include <math.h>

#include "datetime.h"

CUB_NS_BEGIN

struct Candle {
    kid_t    ida;
    price_t  high;
    price_t  low;
    price_t  close;
    price_t  open;
    amnt_t   volume;
    amnt_t   opi;  // opi
    code_t   symbol;
    DateTime dt;

    price_t body() { return fabs( close - open ); }
    price_t long_shadow() { return std::max( upper(), lowers() ); }
    bool    embrace( KLine& o ) { return body_up() >= o.body_up() && body_low() <= o.body_low(); }
    price_t height() { return high - low; }
    price_t bar_mid() { return ( high + low ) / 2; }
    price_t body_mid() { return ( open + close ) / 2; }
    price_t price() { return close; }
    price_t uppers() { return high - body_up(); }
    price_t lowers() { return body_low() - low; }
    price_t body_up() { return std::max( close, open ); }
    price_t body_low() { return std::min( close, open ); }

    bool red() { return close > open; }
    bool black() { return close < open; }
    bool doji() { return fabs( close - open ) < 1e-4; }
    bool white() { return red(); }
    bool green() { return black(); }

    bool close_beyond() { return close > ( high + low ) / 2; }
    bool close_under() { return close < ( high + low ) / 2; }
    bool step_beyond( const KLine& oth_ ) { return high > oth_high && low > oth_.low && close > oth_.close; }
    bool step_below( const KLine& oth_ ) { return low < oth_.low && close < oth_.close && high < oth_.high; }

// k线的开始时间，主要是调试用，确保K线分割合理正确
#ifdef CUB_DEBUG
    int sday;
    int stm;
#endif
};

struct TickSnap {
    Candle bar;
    double bidvol;
    double bidprice;
    double askvol;
    double askprice;
    double highest;
    double lowest;
    double lastclose;  //昨收
    double dayopen;    //今开
    double amount;     //成交额
    double avgprice;
    double upperlimit;
    double lowerlimit;
    double dayclose;  //今收价
};

CUB_NS_END
#endif /* CE4E9C03_2146_4A3B_8E20_5B6B97897D11 */
