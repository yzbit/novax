#ifndef B0FD204B_DBB0_4DB4_BA81_807E439AA053
#define B0FD204B_DBB0_4DB4_BA81_807E439AA053

#include "definitions.h"
#include "indicator.h"
#include "models.h"
#include "ns.h"
#include "series.h"

NVX_NS_BEGIN

struct Kline : IAlgo {
    Kline( const code_t& code_, const period_t& p_, size_t series_count_ );

    void calc( const quotation_t& q_, int bar_count_ ) override;

    candle_t&          bar( int index_ = 0 );
    const quotation_t& qut() const;
    period_t           period() const;
    const code_t&      symbol() const;

private:
    bool is_new_bar( const quotation_t& q_ );

private:
    quotation_t _qut;
    code_t      _symbol;
    period_t    _period;
    datetime_t  _curr_start;  // 当前k的起始时间

private:
    using BarSeries = Series<candle_t>;

    BarSeries* _bars = nullptr;
};

NVX_NS_END

#endif /* B0FD204B_DBB0_4DB4_BA81_807E439AA053 */
