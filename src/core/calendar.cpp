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
#include <iostream>
#include <rapidjson/istreamwrapper.h>
#include <sstream>

#include "calendar.h"

#include "log.hpp"

NVX_NS_BEGIN

calendar::calendar() {
    _year = datetime::now().d.year;
}

bool calendar::is_trade_day() {
    return is_trade_day( datetime().now().d );
}

datespec calendar::previous_day( const datetime& dt_ ) {
    return datetime().from_unix_time( dt_.to_unix_time() - 1 * 24 * 60 * 60 ).d;
}

bool calendar::is_trade_day( const datespec& date_ ) {
    if ( is_weekend( date_ ) ) return false;

    // todo: check if _holiday is empty

    // compare date_ with _holidays
    auto it = _holidays.find( date_.month );
    return it == _holidays.end()
           || std::find( it->second.begin(), it->second.end(), date_.day ) == it->second.end();
}

// 注意：只看trade_day和trade_time不能判断是否交易时间，比如周一凌晨1点day和time都true但不是交易时间，因为这边day和time分开判断的
bool calendar::is_trade_time( const code& c_, const timespec& time_ ) {
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

void calendar::parse_year( const cal_sheet& sh_ ) {
    _year = sh_.HasMember( "year" ) ? sh_[ "year" ].GetInt() : _year;
    LOG_TAGGED( "cal", "get year from calendar: %d", _year );
}

void calendar::parse_hol( const cal_sheet& sh_ ) {
    if ( !sh_.HasMember( "holidays" ) ) {
        LOG_TAGGED( "cal", "no holiday defined" );
        return;
    }

    for ( auto it = sh_[ "holidays" ].MemberBegin(); it != sh_[ "holidays" ].MemberEnd(); ++it ) {
        holiday days;

        int month = std::stoi( it->name.GetString() );
        int i     = 0;

        for ( auto day = it->value.Begin(); day != it->value.End() && i++ < month_days( _year, month ); ++day ) {
            days.emplace_back( day->GetInt() );
        }

        _holidays.try_emplace( month, days );
    }
}

void calendar::parse_sess( const cal_sheet& sh_ ) {
    if ( !sh_.HasMember( "sessions" ) ) {
        LOG_TAGGED( "cal", "no sessions defined" );
        return;
    }

    for ( auto it = sh_[ "sessions" ].MemberBegin(); it != sh_[ "sessions" ].MemberEnd(); ++it ) {
        ins_code code = it->name.GetString();

        ins_session periods;
        periods.reserve( kMaxSessCnt );

        for ( auto sess = it->value.Begin(); sess != it->value.End(); ++sess ) {
            std::string session = sess->GetString();
            auto        pos     = session.find( "-" );
            if ( !session.empty() && std::string::npos == pos ) {
                LOG_TAGGED( "cal", "!!bad session time defined, %s", session.c_str() );
                continue;
            }

            std::string start = session.substr( 0, pos );
            std::string end   = session.substr( pos + 1 );

            if ( start.empty() || end.empty() || std::string::npos == start.find( ":" ) || std::string::npos == end.find( ":" ) ) {
                LOG_TAGGED( "cal", "!!bad session time defined, start=%s,end=%s", start.c_str(), end.c_str() );
                continue;
            }

            std::string start_hour_str   = start.substr( 0, start.find( ":" ) );
            std::string start_minute_str = start.substr( start.find( ":" ) + 1 );
            std::string end_hour_str     = end.substr( 0, end.find( ":" ) );
            std::string end_minute_str   = end.substr( end.find( ":" ) + 1 );

            // if end time is another day (end < start), then split the period
            unsigned temp_start = std::stoi( start_hour_str ) * 100 + std::stoi( start_minute_str );
            unsigned temp_end   = std::stoi( end_hour_str ) * 100 + std::stoi( end_minute_str );

            if ( temp_start / 100 > 23 || temp_start % 100 > 59 || temp_end / 100 > 23 || temp_end % 100 > 59 ) {
                LOG_TAGGED( "cal", "!!bad session time defined, start=%u,end=%u", temp_start, temp_end );
                continue;
            }

            if ( temp_end < temp_start ) {
                periods.push_back( { ( int )temp_start, 2359 } );
                periods.push_back( { 0, ( int )temp_end } );
            }
            else {
                periods.push_back( { ( int )temp_start, ( int )temp_end } );
            }
        }

        assert( periods.size() <= kMaxSessCnt );

        _sessions.try_emplace( code, periods );
    }
}

nvx_st calendar::load_schedule( const char* cal_file_ ) {
    if ( cal_file_ == nullptr )
        return NVX_FAIL;

    std::ifstream ifs( cal_file_ );
    if ( !ifs.is_open() ) {
        LOG_TAGGED( "cal", "open json file failed" );
        return -1;
    }

    rapidjson::IStreamWrapper isw( ifs );
    rapidjson::Document       doc;
    doc.ParseStream( isw );

    if ( doc.HasParseError() ) {
        LOG_TAGGED( "cal", "parse json file failed" );
        return -1;
    }

    parse_year( doc );
    parse_hol( doc );
    parse_sess( doc );

    return NVX_OK;
}

bool calendar::is_weekend( const datespec& d_ ) {
    int wday = d_.wday;

    return wday == 0 || wday == 6;
}

bool calendar::is_trade_datetime( const code& c_ ) {
    return is_trade_datetime( c_, datetime().now() );
}

bool calendar::is_leap_year( int year_ ) {
    return ( 0 == year_ % 100 && 0 == year_ % 400 ) || ( 0 != year_ % 100 && 0 == year_ % 4 );
}

int calendar::month_days( int y_, int m_ ) {
    if ( 1 == m_ || 3 == m_ || 5 == m_ || 7 == m_ || 8 == m_ || 10 == m_ || 12 == m_ )
        return 31;
    else if ( 4 == m_ || 6 == m_ || 9 == m_ || 11 == m_ )
        return 30;
    else
        return is_leap_year( y_ ) ? 29 : 38;
}

bool calendar::is_trade_datetime( const code& c_, const datetime& dt_ ) {
    int hour = dt_.t.hour;

    datetime dt = datetime().from_unix_time( dt_.to_unix_time() );

    if ( hour < 4 ) {
        dt.d = previous_day( dt_ );
    }

    return is_trade_day( dt.d ) && is_trade_time( c_, dt.t );
}

NVX_NS_END
