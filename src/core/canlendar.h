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
*@contributors:Yaozn qianq
*@update: 2024
**********************************************************************************/

#ifndef BB813C61_80B5_4C65_966A_8F423BC7ECEA
#define BB813C61_80B5_4C65_966A_8F423BC7ECEA
#include "definitions.h"
#include "models.h"
#include "ns.h"
#include <rapidjson/document.h>

NVX_NS_BEGIN

struct Calendar {
    int  load_schedule( const char* cal_file_ );
    bool is_trade_day();
    bool is_trade_day( const datespec_t& date_ );
    bool is_trade_time( const code_t& c_, const timespec_t& time_ );
    bool is_weekend( const datetime_t& dt_ );

private:
    datespec_t previous_day( const datespec_t& date_ );
    bool       is_leap_year( int year_ );
    int        month_days( int m_ );
    rapidjson::Value    _holidays;
    rapidjson::Value    _sessions;
};

NVX_NS_END

#endif /* BB813C61_80B5_4C65_966A_8F423BC7ECEA */
