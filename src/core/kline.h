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

struct kline : algo {
    kline( const code& code_, const Period& p_, size_t series_count_ );

    void calc( const tick& q_, int bar_count_ ) override;

    candle&     bar( int index_ = 0 );
    const tick& qut() const;
    Period      period() const;
    const code& symbol() const;

private:
    bool is_new_bar( const tick& q_ );

private:
    tick     _qut;
    code     _symbol;
    Period   _period;
    datetime _curr_start;  // 当前k的起始时间

private:
    using BarSeries = series<candle_t>;

    BarSeries* _bars = nullptr;
};

NVX_NS_END

#endif /* B0FD204B_DBB0_4DB4_BA81_807E439AA053 */
