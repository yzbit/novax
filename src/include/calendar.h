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

struct calendar {
    static calendar& instance();

    nvx_st load_schedule( const char* cal_file_ );
    bool   is_trade_day();
    bool   is_trade_day( const datespec& date_ );
    bool   is_trade_time( const code& c_, const timespec& time_ );
    bool   is_weekend( const datespec& d_ );
    bool   is_trade_datetime( const code& c_ );
    bool   is_trade_datetime( const code& c_, const datetime& dt_ );

    calendar();

private:
    datespec previous_day( const datetime& dt_ );
    bool     is_leap_year( int year_ );
    int      month_days( int y_, int m_ );

private:
    using cal_sheet = rapidjson::Document;
    void parse_year( const cal_sheet& sh_ );
    void parse_hol( const cal_sheet& sh_ );
    void parse_sess( const cal_sheet& sh_ );

private:
    static constexpr int MAX_SESSION_CNT = 10;

    struct session {
        int start, end;
    };

    using ins_session  = std::vector<session>;
    using session_repo = std::map<ins_code, ins_session>;

    using holiday      = std::vector<int>;
    using holiday_repo = std::map<int, holiday>;

    int          _year;
    holiday_repo _holidays;
    session_repo _sessions;
};

NVX_NS_END

#define CAL NVX_NS::calendar::instance()

#endif /* BB813C61_80B5_4C65_966A_8F423BC7ECEA */
