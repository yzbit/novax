#ifndef C096ECC6_D3E8_4656_A4DF_F125629A8BE4
#define C096ECC6_D3E8_4656_A4DF_F125629A8BE4

#include <algorithm>
#include <any>
#include <array>
#include <assert.h>
#include <functional>
#include <memory>
#include <optional>
#include <stdint.h>
#include <string.h>
#include <string>
#include <time.h>

#include "ns.h"

CUB_NS_BEGIN

using id_t     = uint32_t;
using price_t  = double;
using vol_t    = int;  // todo double ctp都是整数仓位;
using oid_t    = id_t;
using text_t   = std::string;
using string_t = std::string;
using money_t  = double;

#define IS_VALID_ID( _id_ ) ( 0 != ( _id_ ) )

struct code_t {
    static constexpr int kMaxCodeLength = 16;

    code_t( const char* code_ )
        : code{ 0 } {
        memcpy( code, code_, std::min( sizeof( code ) - 1, strlen( code_ ) ) );
    }

    code_t( const std::string& str_ )
        : code_t{ str_.c_str() } {
    }

    code_t( int c_ ) {
        sprintf( code, "%d", c_ );
    }

    code_t() = default;

    code_t& operator=( const code_t& c_ ) {
        memcpy( code, c_.code, sizeof( code ) );
        return *this;
    }

    bool operator==( const code_t& c_ ) {
        return memcmp( code, c_.code, sizeof( code ) ) == 0;
    }

    bool operator!=( const code_t& c_ ) {
        return !( *this == c_ );
    }

    operator char*() {
        return code;
    }

    operator const char*() const {
        return code;
    }

    char code[ kMaxCodeLength ] = { 0 };
};

struct code_hash_t {
    std::size_t operator()( const code_t& c_ ) const {
        // todo?
        return std::hash<uint64_t>()( *( uint64_t* )&c_[ 0 ] ) ^ std::hash<uint64_t>()( *( uint64_t* )&c_[ 8 ] );
    }
};

struct datetime_t {
    int year, month, day, wday, hour, minute, seconds, milli;

    void from_unix_time( const time_t& t_ ) {
        auto tm = localtime( &t_ );
        year    = tm->tm_year + 1900;
        month   = tm->tm_mon + 1;
        day     = tm->tm_mday;
        wday    = tm->tm_wday;
        hour    = tm->tm_hour;
        minute  = tm->tm_min;
        seconds = tm->tm_sec;

        milli = 0;
    }

    time_t to_unix_time() const {
        struct tm t;
        t.tm_year = year - 1900;
        t.tm_mon  = month - 1;
        t.tm_mday = day;
        t.tm_hour = hour;
        t.tm_min  = minute;
        t.tm_sec  = seconds;

        return mktime( &t );
    }

    bool is_valid() {
        return day != 0;
    }

#define TK_DIFF ( 1000 + 100 + 10 + 1 ) * '0'
#define H_DIFF ( 10 + 1 ) * '0'

    // YYYYMMDD，HH:MM:SS，MILLI ctp独有的格式
    void from_ctp( const char* day_, const char* time_, int milli_ ) {
        // year  = ( day_[ 0 ] - '0' ) * 1000 + ( day_[ 1 ] - '0' ) * 100 + ( day_[ 2 ] - '0' ) * 10 + ( day_[ 3 ] - '0' );
        // month = ( day_[ 4 ] - '0' ) * 10 + ( day_[ 5 ] - '0' );
        // day   = ( day_[ 6 ] - '0' ) * 10 + ( day_[ 7 ] - '0' );
        year  = day_[ 0 ] * 1000 + day_[ 1 ] * 100 + day_[ 2 ] * 10 + day_[ 3 ] - TK_DIFF;
        month = day_[ 4 ] * 10 + day_[ 5 ] - H_DIFF;
        day   = day_[ 6 ] * 10 + day_[ 7 ] - H_DIFF;

        // hour    = ( time_[ 0 ] - '0' ) * 10 + ( time_[ 1 ] - '0' );
        // minute  = ( time_[ 3 ] - '0' ) * 10 + ( time_[ 4 ] - '0' );
        // seconds = ( time_[ 6 ] - '0' ) * 10 + ( time_[ 7 ] - '0' );
        hour    = time_[ 0 ] * 10 + time_[ 1 ] - H_DIFF;
        minute  = time_[ 3 ] * 10 + time_[ 4 ] - H_DIFF;
        seconds = time_[ 6 ] * 10 + time_[ 7 ] - H_DIFF;

        milli = milli_;
        // w=y+[y/4]+[c/4]-2c+[26(m+1)/10]+d-1
        // 2049: y=49, c=20,-0 星期日
        // wday = ( year % 100 + ( year % 100 ) / 4 + ( year / 100 ) / 4 - 2 * ( year / 100 ) + 26 * ( month + 1 ) / 10 + day - 1 ) % 7;
    }

    std::string to_iso() const {
        char fmt[ 64 ];
        sprintf( fmt, "%04d-%02d-%02d %02d:%02d:%02d.%03dZ", year, month, day, hour, minute, seconds, milli );

        return fmt;
    }
};

using ex_t = code_t;  // 交易所

#define DAY_SECONDS ( 24 * 3600u )
#define WEEK_SECONDS ( 7 * 24 * 3600u )
#define MONTH_SECONDS ( 31 * 3600u )
#define YEAR_SECONDS ( 266 * 24 * 3600u )
struct period_t {
    enum class type_t : uint8_t {
        milli,
        seconds,
        min,
        hour,
        day,
        week,
        month,
        year
    };

    // todo note: 注意：如果t=year，rep > 1是没有意义的
    period_t( const type_t& t_, int r_ )
        : t( t_ )
        , rep( r_ ) {

        assert( ( uint32_t )( *this ) <= 1 * 365 * 24 * 3600 );
    }

    // 转成秒--时间粒度不仅仅要和绝对时间有关系还和分割边界有关系，比如1天显然不能按照绝对秒数来算，应该按照收盘时间和开盘时间来算，每周则只能基于小时来算,分钟其实会出现跨天的状况,可以按照秒来算，同时按照自然阅读来分
    operator uint32_t() {
        switch ( t ) {
        case type_t::milli: return 0 == rep / 1000 ? 1 : rep / 1000;
        case type_t::seconds: return rep;
        case type_t::min: return rep * 60;
        case type_t::hour: return rep * 3600;
        case type_t::day: return rep * DAY_SECONDS;
        case type_t::week: return rep * WEEK_SECONDS;
        case type_t::month: return rep * MONTH_SECONDS;
        case type_t::year: return rep * YEAR_SECONDS;
        default:
            break;
        }
        return 0;
    }

    type_t t;
    int    rep;
};

template <typename T>
struct array_t {
    array_t( int n )
        : _size( n )
        , _data( nullptr ) {
        _data = std::make_unique<T[]>( n );
    }

    T& operator[]( int index_ ) {
        return get( index_ );
    }

    T& get( int index_ ) {
        assert( ( uint32_t )index_ < _size );
        return element( index_ );
    }

    void set( const T& t_, int index_ ) {
        assert( ( uint32_t )index_ < _size );
        element[ index_ ] = t_;
    }

    void set( const T&& t_, int index_ ) {
        assert( ( uint32_t )index_ < _size );
        element[ index_ ] = t_;
    }

    int size() { return _size; }

    void for_each( std::function<bool( T& t_ )> op_ ) {
        int i = 0;
        while ( i < _size && op_( element( i++ ) ) )
            ;
    }

private:
    bool is_valid( int index_ ) {
        return ( uint32_t )index_ < _size;
    }

    T& element( int index_ ) {
        return _data.get()[ index_ ];
    }

private:
    int                _size;
    std::unique_ptr<T> _data;
};

#define _( _literal_ ) std::string( _literal_ )

#define THREAD_DETACHED( _func_ ) std::thread( _func_ ).detach()
#define THREAD_JOINED( _func_ ) std::thread( _func_ ).join()

struct arg_t {
    std::any value;

    operator const int() const {
        try {
            return std::any_cast<int>( value );
        }
        catch ( ... ) {
            return 0;
        }
    }

    operator const double() const {
        try {
            return std::any_cast<double>( value );
        }
        catch ( ... ) {
            return 0.0;
        }
    }

    operator const char*() const {
        try {
            return std::any_cast<const char*>( value );
        }
        catch ( ... ) {
            return "";
        }
    }

    operator const period_t() const {
        try {
            return std::any_cast<const period_t>( value );
        }
        catch ( ... ) {
            return period_t( period_t::type_t::min, 1 );
        }
    }

    operator string_t() const {
        try {
            return std::any_cast<string_t>( value );
        }
        catch ( ... ) {
            return string_t( ( const char* )*this );
        }
    }
};

#define MAX_ARG_SUPPORT 16
using arg_pack_t = std::array<arg_t, MAX_ARG_SUPPORT>;

struct Indicator;

using algo_creator_t = std::function<Indicator*( const arg_pack_t& a )>;

struct algo_t {
    string_t       name;
    algo_creator_t creator;
};

CUB_NS_END

#endif /* C096ECC6_D3E8_4656_A4DF_F125629A8BE4 */
