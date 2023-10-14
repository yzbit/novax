#ifndef B0FD204B_DBB0_4DB4_BA81_807E439AA053
#define B0FD204B_DBB0_4DB4_BA81_807E439AA053

#include "definitions.h"
#include "indicator.h"
#include "models.h"
#include "ns.h"

CUB_NS_BEGIN

struct Series;
struct Kline : Indicator {
    static Kline* create( const arg_pack_t& arg_ );

    void      on_calc( const quotation_t& q_ ) override;
    candle_t& bar( int index_ = 0 );

private:
    Kline( const code_t& code_, const period_t& p_, int series_count_ );
    void init();

private:
    bool is_new_bar( const quotation_t& q_ );

private:
    int        _count;
    code_t     _symbol;
    period_t   _period;
    int        _curr_bar;
    datetime_t _curr_start;  // 当前k的起始时间

private:
    Series* _data;
};

CUB_NS_END

#endif /* B0FD204B_DBB0_4DB4_BA81_807E439AA053 */
