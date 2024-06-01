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

* \author: qianq(695997058@qq.com)
* \date: 2024
**********************************************************************************/

#include <gtest/gtest.h>
#include <novax.h>
#include <stdio.h>

TEST( calendar, Basic ) {
    NVX_NS::calendar c;
    ASSERT_TRUE( c.load_schedule( 0 ) < 0 );

    ASSERT_TRUE( c.load_schedule( "conf.d/ctp/cal.json" ) == 0 );

    NVX_NS::datespec date;
    date.year  = 2024;
    date.month = 5;
    date.day   = 6;
    date.wday  = 1;

    NVX_NS::timespec time;
    time.hour   = 1;
    time.minute = 29;
    time.second = 10;
    time.milli  = 399;

    NVX_NS::datetime dt;
    dt.d = date;
    dt.t = time;

    NVX_NS::code code = "sc2410";

    ASSERT_TRUE( c.is_trade_day( date ) );
    ASSERT_TRUE( c.is_trade_time( code, time ) );
    ASSERT_FALSE( c.is_trade_datetime( code, dt ) );
}
