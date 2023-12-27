#ifndef C096ECC6_D3E8_4656_A4DF_F125629A8BE4
#define C096ECC6_D3E8_4656_A4DF_F125629A8BE4

#include <algorithm>
#include <any>
#include <array>
#include <assert.h>
#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <stdint.h>
#include <string.h>
#include <string>
#include <thread>
#include <time.h>

#include "ns.h"

SATURN_NS_BEGIN

#define CUB_ASSERT assert

using id_t     = uint32_t;
using price_t  = double;
using vol_t    = int;  // todo double ctp都是整数仓位;
using oid_t    = id_t;
using string_t = std::string;
using text_t   = string_t;
using money_t  = double;

constexpr int kBadId = 0;
#define IS_VALID_ID( _id_ ) ( kBadId != ( _id_ ) )

inline uint64_t now_ms() {
    return std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now().time_since_epoch() ).count();
}

enum class pricetype_t {
    open,
    close,
    high,
    low,
    mid,
    avg
};

struct code_t {
    static constexpr int kMaxCodeLength = 16;

    code_t( const char* code_ );
    code_t( const std::string& str_ );
    code_t( int c_ );
    code_t() = default;
    const char* c_str() const;

    bool    empty() const;
    code_t& operator=( const code_t& c_ );
    bool    operator==( const code_t& c_ );
    bool    operator!=( const code_t& c_ );

    operator char*();
    operator const char*() const;

private:
    char _code[ kMaxCodeLength ] = { 0 };
};

struct code_hash_t {
    std::size_t operator()( const code_t& c_ ) const {
        // todo?
        static_assert( sizeof( code_t ) == 16, "bad code size ,not 16" );
        return std::hash<uint64_t>()( *( uint64_t* )&c_[ 0 ] ) ^ std::hash<uint64_t>()( *( uint64_t* )&c_[ 8 ] );
    }
};

struct datetime_t {
    int year, month, day, wday, hour, minute, seconds, milli;

    static datetime_t now();
    void              from_unix_time( const time_t& t_ );
    time_t            to_unix_time() const;
    bool              is_valid() const;
    void              from_ctp( const char* day_, const char* time_, int milli_ );
    string_t          to_iso() const;
};

struct time_range_t {
    datetime_t start;
    datetime_t end;
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
    period_t( const type_t& t_, int r_ );
    // 转成秒--时间粒度不仅仅要和绝对时间有关系还和分割边界有关系，比如1天显然不能按照绝对秒数来算，应该按照收盘时间和开盘时间来算，每周则只能基于小时来算,分钟其实会出现跨天的状况,可以按照秒来算，同时按照自然阅读来分
    operator uint32_t();

    type_t t;
    int    rep;
};

template <typename T>
struct array_t {
    array_t( int n );

    T&   operator[]( int index_ );
    T&   get( int index_ );
    void set( const T& t_, int index_ );
    void set( const T&& t_, int index_ );
    int  size() const;
    void for_each( std::function<bool( T& t_ )> op_ );

private:
    bool is_valid( int index_ ) const;
    T&   element( int index_ );

private:
    int                _size;
    std::unique_ptr<T> _data;
};

#define _( _literal_ ) std::string( _literal_ )

#define THREAD_DETACHED( _func_ ) std::thread( _func_ ).detach()
#define THREAD_JOINED( _func_ ) std::thread( _func_ ).join()
#define THREAD_SPAWN( _func_, _thread_ ) \
    do {                                 \
        std::thread _t( _func_ );        \
        if ( _thread_ )                  \
            _thread_->swap( _t );        \
        else                             \
            _t.detach();                 \
    } while ( 0 )

struct arg_t {
    std::any value;

    operator const int() const;
    operator const double() const;
    operator const char*() const;
    operator string_t() const;
    operator const period_t() const;
};

#define MAX_ARG_SUPPORT 16
using arg_pack_t = std::array<arg_t, MAX_ARG_SUPPORT>;

struct Indicator;

using algo_creator_t = std::function<Indicator*( const arg_pack_t& a )>;

struct algo_t {
    string_t       name;
    algo_creator_t creator;
};

//--inlines------------------------------------------------------------------------
inline code_t::code_t( const char* code_ )
    : _code{ 0 } {
    memcpy( _code, code_, std::min( sizeof( _code ) - 1, strlen( code_ ) ) );
}

inline const char* code_t::c_str() const {
    return _code;
}

inline code_t::code_t( const std::string& str_ )
    : code_t{ str_.c_str() } {
}

inline code_t::code_t( int c_ ) {
    sprintf( _code, "%d", c_ );
}

inline bool code_t::empty() const {
    return _code[ 0 ] == '\0';
}

inline code_t& code_t::operator=( const code_t& c_ ) {
    memcpy( _code, c_._code, sizeof( _code ) );
    return *this;
}

inline bool code_t::operator==( const code_t& c_ ) {
    return memcmp( _code, c_._code, sizeof( _code ) ) == 0;
}

inline bool code_t::operator!=( const code_t& c_ ) {
    return !( *this == c_ );
}

inline code_t::operator char*() {
    return _code;
}

inline code_t::operator const char*() const {
    return _code;
}
//
// todo note: 注意：如果t=year，rep > 1是没有意义的
inline period_t::period_t( const type_t& t_, int r_ )
    : t( t_ )
    , rep( r_ ) {

    CUB_ASSERT( ( uint32_t )( *this ) <= 1 * 365 * 24 * 3600 );
}

// 转成秒--时间粒度不仅仅要和绝对时间有关系还和分割边界有关系，比如1天显然不能按照绝对秒数来算，应该按照收盘时间和开盘时间来算，每周则只能基于小时来算,分钟其实会出现跨天的状况,可以按照秒来算，同时按照自然阅读来分
inline period_t::operator uint32_t() {
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

//
template <typename T>
inline array_t<T>::array_t( int n )
    : _size( n )
    , _data( nullptr ) {
    _data = std::make_unique<T[]>( n );
}

template <typename T>
inline T& array_t<T>::operator[]( int index_ ) {
    return get( index_ );
}

template <typename T>
inline T& array_t<T>::get( int index_ ) {
    CUB_ASSERT( ( uint32_t )index_ < _size );
    return element( index_ );
}

template <typename T>
inline void array_t<T>::set( const T& t_, int index_ ) {
    CUB_ASSERT( ( uint32_t )index_ < _size );
    element[ index_ ] = t_;
}

template <typename T>
inline void array_t<T>::set( const T&& t_, int index_ ) {
    CUB_ASSERT( ( uint32_t )index_ < _size );
    element[ index_ ] = t_;
}

template <typename T>
inline int array_t<T>::size() const { return _size; }

template <typename T>
inline void array_t<T>::for_each( std::function<bool( T& t_ )> op_ ) {
    int i = 0;
    while ( i < _size && op_( element( i++ ) ) )
        ;
}

template <typename T>
inline bool array_t<T>::is_valid( int index_ ) const {
    return ( uint32_t )index_ < _size;
}

template <typename T>
inline T& array_t<T>::element( int index_ ) {
    return _data.get()[ index_ ];
}

//
inline arg_t::operator const int() const {
    try {
        return std::any_cast<int>( value );
    }
    catch ( ... ) {
        return 0;
    }
}

inline arg_t::operator const double() const {
    try {
        return std::any_cast<double>( value );
    }
    catch ( ... ) {
        return 0.0;
    }
}

inline arg_t::operator const char*() const {
    try {
        return std::any_cast<const char*>( value );
    }
    catch ( ... ) {
        return "";
    }
}

inline arg_t::operator const period_t() const {
    try {
        return std::any_cast<const period_t>( value );
    }
    catch ( ... ) {
        return period_t( period_t::type_t::min, 1 );
    }
}

inline arg_t::operator string_t() const {
    try {
        return std::any_cast<string_t>( value );
    }
    catch ( ... ) {
        return string_t( ( const char* )*this );
    }
}

inline datetime_t datetime_t::now() {
    datetime_t t;
    t.from_unix_time( time( 0 ) );

    return t;
}

inline void datetime_t::from_unix_time( const time_t& t_ ) {
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

inline time_t datetime_t::to_unix_time() const {
    struct tm t;
    t.tm_year = year - 1900;
    t.tm_mon  = month - 1;
    t.tm_mday = day;
    t.tm_hour = hour;
    t.tm_min  = minute;
    t.tm_sec  = seconds;

    return mktime( &t );
}

inline bool datetime_t::is_valid() const {
    return day != 0;
}

#define TK_DIFF ( 1000 + 100 + 10 + 1 ) * '0'
#define H_DIFF ( 10 + 1 ) * '0'

// YYYYMMDD，HH:MM:SS，MILLI ctp独有的格式
inline void datetime_t::from_ctp( const char* day_, const char* time_, int milli_ ) {
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

inline std::string datetime_t::to_iso() const {
    char fmt[ 64 ];
    sprintf( fmt, "%04d-%02d-%02d %02d:%02d:%02d.%03dZ", year, month, day, hour, minute, seconds, milli );

    return fmt;
}
SATURN_NS_END

#endif /* C096ECC6_D3E8_4656_A4DF_F125629A8BE4 */
