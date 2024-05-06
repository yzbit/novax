/************************************************************************************
The MIT License

Copyright (c) 2024 YaoZinan  [zinan@outlook.com, nvx-quant.com]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

* \author: yaozn(zinan@outlook.com)
* \date: 2024
**********************************************************************************/

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
