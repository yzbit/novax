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

* \author: yaozn(zinan@outlook.com) , qianq(695997058@qq.com)
* \date: 2024
**********************************************************************************/

#ifndef BB813C61_80B5_4C65_966A_8F423BC7ECEA
#define BB813C61_80B5_4C65_966A_8F423BC7ECEA
#include <array>
#include <map>
#include <rapidjson/document.h>
#include <string>
#include <vector>

#include "definitions.h"
#include "models.h"
#include "ns.h"

NVX_NS_BEGIN

struct Calendar {
    nvx_st load_schedule( const char* cal_file_ );
    bool   is_trade_day();
    bool   is_trade_day( const datespec_t& date_ );
    bool   is_trade_time( const code_t& c_, const timespec_t& time_ );
    bool   is_weekend( const datespec_t& d_ );
    bool   is_trade_datetime( const code_t& c_ );
    bool   is_trade_datetime( const code_t& c_, const datetime_t& dt_ );

    Calendar();

private:
    datespec_t previous_day( const datetime_t& dt_ );
    bool       is_leap_year( int year_ );
    int        month_days( int y_, int m_ );

private:
    using CalSheet = rapidjson::Document;
    void parse_year( const CalSheet& sh_ );
    void parse_hol( const CalSheet& sh_ );
    void parse_sess( const CalSheet& sh_ );

private:
    static constexpr int kMaxSessCnt = 10;

    struct sess_t {
        int start, end;
    };

    using InsSession  = std::vector<sess_t>;
    using SessionRepo = std::map<ins_t, InsSession>;

    using Holiday     = std::vector<int>;
    using HolidayRepo = std::map<int, Holiday>;
    int         _year;
    HolidayRepo _holidays;
    SessionRepo _sessions;
};

NVX_NS_END

#endif /* BB813C61_80B5_4C65_966A_8F423BC7ECEA */
