#include <fstream>
#include <rapidjson/istreamwrapper.h>
#include <sstream>

#include "canlendar.h"

#include "log.hpp"

NVX_NS_BEGIN

bool Calendar::is_trade_day() {
    return is_trade_day( datetime().now().d );
}

bool Calendar::is_trade_day( const datespec_t& date_ ) {
    // todo: check if _holiday is empty

    // compare date_ with _holidays
    std::stringstream month_ss, day_ss;
    month_ss << date_.month;
    day_ss << date_.day;
    std::string month = month_ss.str();
    std::string day   = day_ss.str();

    if ( _holidays.HasMember( month.c_str() ) ) {
        auto& days = _holidays[ month.c_str() ];
        // _holiday[month]: [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        for ( auto it = days.Begin(); it != days.End(); ++it ) {
            if ( *it == date_.day ) {
                return false;
            }
        }
    }

    return true;
}

bool Calendar::is_trade_time( const code_t& c_, const timespec_t& time_ ) {
    // todo: check if _sessions is empty

    // compare time_ with _sessions
    std::stringstream hour_ss, minute_ss;
    hour_ss << time_.hour;
    minute_ss << time_.minute;
    std::string hour   = hour_ss.str();
    std::string minute = minute_ss.str();

    // _sessions[c_]: ["9:00-10:15", "10:30-11:30", "13:30-15:00", "21:00-23:00"]
    if ( _sessions.HasMember( c_.c_str() ) ) {
        auto& sessions = _sessions[ c_.c_str() ];
        for ( auto it = sessions.Begin(); it != sessions.End(); ++it ) {
            std::string session = it->GetString();
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

            if ( time_.hour >= std::stoi( start_hour_str ) && time_.hour <= std::stoi( end_hour_str ) ) {
                if ( time_.hour == std::stoi( start_hour_str ) ) {
                    if ( time_.minute >= std::stoi( start_minute_str ) ) {
                        return true;
                    }
                }
                else if ( time_.hour == std::stoi( end_hour_str ) ) {
                    if ( time_.minute <= std::stoi( end_minute_str ) ) {
                        return true;
                    }
                }
                else {
                    return true;
                }
            }
        }
    }

    return false;
}

int Calendar::load_schedule( const char* cal_file_ ) {
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

    _holidays = doc[ "holidays" ];
    _sessions = doc[ "sessions" ];

    return 0;
}

bool Calendar::is_weekend( const datetime_t& dt_ ) {
    int wday = dt_.d.wday;

    return wday == 0 || wday == 6;
}

NVX_NS_END
