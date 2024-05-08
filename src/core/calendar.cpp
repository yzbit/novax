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

#include <fstream>
#include <rapidjson/istreamwrapper.h>
#include <sstream>

#include "calendar.h"

#include "log.hpp"

NVX_NS_BEGIN

bool Calendar::is_trade_day() {
    return is_trade_day( datetime_t().now().d );
}

datespec_t Calendar::previous_day( const datespec_t& date_ ) {
    return datespec_t();
}

bool Calendar::is_trade_day( const datespec_t& date_ ) {
    // todo: check if _holiday is empty

    // compare date_ with _holidays
    auto it = _holidays.find( date_.month );
    return it != _holidays.end()
           && std::find( it->second.begin(), it->second.end(), date_.day ) != it->second.end();
}

// 注意：只看trade_day和trade_time不能判断是否交易时间，比如周一凌晨1点day和time都true但不是交易时间，因为这边day和time分开判断的
bool Calendar::is_trade_time( const code_t& c_, const timespec_t& time_ ) {
    // todo: check if _sessions is empty

    // compare time_ with _sessions
    std::stringstream hour_ss, minute_ss;
    hour_ss << time_.hour;
    minute_ss << time_.minute;
    std::string hour   = hour_ss.str();
    std::string minute = minute_ss.str();

    auto it = _sessions.find( code2ins( c_ ) );
    if ( it == _sessions.end() ) return false;

    for ( auto& period : it->second ) {
        if ( time_.hour * 100 + time_.minute >= period.start && time_.hour * 100 + time_.minute <= period.end ) {
            return true;
        }
    }

    return false;
}

nvx_st Calendar::load_schedule( const char* cal_file_ ) {
    // set defalut value src/core/ctp/ctp.cal.json
    if ( cal_file_ == nullptr ) {
        cal_file_ = "ctp/ctp.cal.json";
    }

    std::ifstream             ifs( cal_file_ );
    rapidjson::IStreamWrapper isw( ifs );
    rapidjson::Document       doc;
    doc.ParseStream( isw );

    if ( doc.HasParseError() ) {
        LOG_TAGGED( "cal", "parse json file failed" );
        return -1;
    }

    for ( auto it = doc[ "holidays" ].MemberBegin(); it != doc[ "holidays" ].MemberEnd(); ++it ) {
        Holiday days;

        int i = 0;
        for ( auto day = it->value.Begin(); day != it->value.End() && i < Calendar::kMaxHol; ++day ) {
            days[ i++ ] = day->GetInt();
        }
        _holidays.try_emplace( it->name.GetInt(), days );
    }

    for ( auto it = doc[ "sessions" ].MemberBegin(); it != doc[ "sessions" ].MemberEnd(); ++it ) {
        ins_t      code = it->name.GetString();
        InsSession periods;
        int        i = 0;
        for ( auto sess = it->value.Begin(); sess != it->value.End(); ++sess ) {
            std::string session = sess->GetString();
            auto        pos     = session.find( "-" );
            if ( pos == std::string::npos ) {
                continue;
            }

            std::string start = session.substr( 0, pos );
            std::string end   = session.substr( pos + 1 );

            std::string start_hour_str   = start.substr( 0, start.find( ":" ) );
            std::string start_minute_str = start.substr( start.find( ":" ) + 1 );
            std::string end_hour_str     = end.substr( 0, end.find( ":" ) );
            std::string end_minute_str   = end.substr( end.find( ":" ) + 1 );

            // if end time is another day (end < start), then split the period
            int temp_start = std::stoi( start_hour_str ) * 100 + std::stoi( start_minute_str );
            int temp_end   = std::stoi( end_hour_str ) * 100 + std::stoi( end_minute_str );
            if ( temp_end < temp_start ) {
                periods[ i ].start = temp_start;
                periods[ i ].end   = 2359;
                i++;
                periods[ i ].start = 0;
                periods[ i ].end   = temp_end;
                i++;
                continue;
            }
            else {
                periods[ i ].start = temp_start;
                periods[ i ].end   = temp_end;
                i++;
            }
        }

        _sessions.try_emplace( code2ins( code ), periods );
    }

    return NVX_OK;
}

bool Calendar::is_weekend( const datetime_t& dt_ ) {
    int wday = dt_.d.wday;

    return wday == 0 || wday == 6;
}

bool Calendar::is_trade_datetime( const code_t& c_ ) {
    return is_trade_datetime( c_, datetime_t().now() );
}

bool Calendar::is_trade_datetime( const code_t& c_, const datetime_t& datetime_ ) {
    int hour = datetime_.t.hour;

    datetime_t dt = datetime_t().from_unix_time( datetime_.to_unix_time() );

    if ( hour < 4 ) {
        dt.d = previous_day( dt.d );
    }

    return is_trade_day( dt.d ) && is_trade_time( c_, dt.t );
}

NVX_NS_END
