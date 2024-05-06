/************************************************************************************
MIT License

Copyright (c) 2024 [YaoZinan zinan@outlook.com nvx-quant.com]

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

*@init: Yaozn
*@contributors:Yaozn
*@update: 2024
**********************************************************************************/

#include <stdio.h>

#include "ma.h"

NVX_NS_BEGIN

Ma* Ma::create( const arg_pack_t& p_ ) {
    return new Ma( p_[ 0 ], ( int )p_[ 1 ] );
}

Ma::Ma( const std::string& code_, int period_ ) {
    fprintf( stderr, "%s %d\n", code_.c_str(), period_ );
    add_series( 1, period_ );
}

void Ma::on_calc( const Kline& ref_, const quotation_t& q_ ) {
    if ( ref_.bar_cnt() < period_ ) {
        return 0;
    }

    double accum = 0;

    // 如果bar移动位置了,则移动平均也要移动,否则就原地计算
    accum += q_.last;

    //从当前的bar计算到ref_bar.cnt
    for ( int start = this->bar_cnt(); start < ref_.bar_cnt(); ++start ) {

    }
    //
}

NVX_NS_END