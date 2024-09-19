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

* \author: yaozn(zinan@outlook.com)
* \date: 2024
**********************************************************************************/

#ifndef C096ECC6_D3E8_4656_A4DF_F125629A8BE4
#define C096ECC6_D3E8_4656_A4DF_F125629A8BE4

#include <algorithm>
#include <any>
#include <array>
#include <assert.h>
#include <chrono>
#include <cstdint>
#include <ctype.h>
#include <functional>
#include <memory>
#include <optional>
#include <stdint.h>
#include <string.h>
#include <string>
#include <thread>
#include <time.h>
#include <tuple>
#include <vector>

#include "ns.h"

NVX_NS_BEGIN

#define NVX_ASSERT assert

using nvx_st = int;
// using id_t         = uint32_t;
using price       = float;
using vol         = int;  // todo double ctp都是整数仓位;
using oid         = int;
using xstring     = std::string;
using string_list = std::vector<std::string>;
using text        = xstring;
using money       = float;
using kidx        = uint32_t;

enum class nvxerr {
    order_rejected,
};

constexpr nvx_st NVX_OK   = 0;
constexpr nvx_st NVX_FAIL = -1;

constexpr oid NVX_BAD_OID = -1;

#define IS_VALID_OID( _id_ ) ( _id_ > 0 )

inline uint64_t now_ms() {
    return std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now().time_since_epoch() ).count();
}

inline bool is_nil( const char* sz_ ) {
    return !sz_ || 0 == sz_[ 0 ];
}

enum class pricetype_t {
    open,
    close,
    high,
    low,
    mid,
    avg
};

struct code {
    static constexpr int MAX_CODE_LENGTH = 8;

    code( const char* code_ );
    code( const xstring& str_ );
    code( int c_ );
    code() = default;
    char*  c_str() const;
    size_t length() const;

    bool  empty() const;
    code& operator=( const code& c_ );

    bool operator==( const code& c_ ) const;
    bool operator!=( const code& c_ ) const;
    bool operator!=( const char* c_ ) const;
    bool operator==( const char* c_ ) const;
    bool operator<( const code& c_ ) const;

    operator bool() const { return !!_code[ 0 ]; }

private:
    char _code[ MAX_CODE_LENGTH + 1 ] = { 0 };
};

using ins_code = code;
inline ins_code code2ins( const code& c_ ) {
    char ins[ code::MAX_CODE_LENGTH + 1 ] = { 0 };

    const char* sz  = c_.c_str();
    char*       dst = ins;
    while ( *sz && !std::isdigit( *sz ) )
        *dst++ = *sz++;

    return ins_code( ins );
}

struct code_hash {
    std::size_t operator()( const code& c_ ) const {
        static_assert( sizeof( code ) == 9, "bad code size ,not 8" );
        return std::hash<uint64_t>()( *( uint64_t* )&c_.c_str()[ 0 ] );
    }
};

struct datespec {
    int year, month, day, wday;
};

struct timespec {
    int hour, minute, second, milli;
};

struct datetime {
    datespec d;
    timespec t;

    static datetime now();
    datetime&       from_unix_time( const time_t& t_ );
    time_t          to_unix_time() const;
    bool            is_valid() const;
    void            from_ctp( const char* day_, const char* time_, int milli_ );
    xstring         to_iso() const;
    void            print( const char* prefix = "", FILE* fp_ = nullptr ) const;
};

struct time_range_t {
    datetime start;
    datetime end;
};

using exch = code;  // 交易所
using exid = int;

#define DAY_SECONDS ( 24 * 3600l )
#define WEEK_SECONDS ( 7 * 24 * 3600l )
#define MONTH_SECONDS ( 31 * 3600l )
#define YEAR_SECONDS ( 266 * 24 * 3600l )

struct period {
    enum class base : uint8_t {
        milli,
        second,
        min,
        hour,
        day,
        week,
        month,
        year
    };

    constexpr period( int r_, base b_ )
        : r( r_ )
        , b( b_ ) {}

    int  r = 0;
    base b = base::second;
};

constexpr period pd_one_second     = period( 1, period::base::second );
constexpr period pd_five_seconds   = period( 5, period::base::second );
constexpr period pd_ten_seconds    = period( 10, period::base::second );
constexpr period pd_thirty_seconds = period( 30, period::base::second );
constexpr period pd_one_minute     = period( 1, period::base::min );
constexpr period pd_five_minutes   = period( 5, period::base::min );
constexpr period pd_ten_minutes    = period( 10, period::base::min );
constexpr period pd_thrity_minutes = period( 10, period::base::min );
constexpr period pd_one_hour       = period( 1, period::base::hour );
constexpr period pd_two_hours      = period( 2, period::base::hour );
constexpr period pd_four_hours     = period( 4, period::base::hour );
constexpr period pd_eight_hours    = period( 8, period::base::hour );
constexpr period pd_one_day        = period( 1, period::base::day );
constexpr period pd_one_week       = period( 1, period::base::week );
constexpr period pd_one_month      = period( 1, period::base::month );
constexpr period pd_one_year       = period( 1, period::base::year );

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

template <typename... Ts>
struct arg_t {
    arg_t( Ts... ts )
        : _tuple( ts... ) {}

    auto& get( std::size_t i ) {
        return std::get<i>( _tuple );
    }

    static constexpr size_t count = std::tuple_size_v<std::tuple<Ts...>>;

    template <typename F>
    void for_each( F f ) {
        for_each_impl( f, std::make_index_sequence<sizeof...( Ts )>{} );
    }

private:
    template <typename F, size_t... N>
    void for_each_impl( F f, std::index_sequence<N...> ) {
        ( std::invoke( f, std::get<N>( _tuple ) ), ... );
    }

    std::tuple<Ts...> _tuple;
};

#define DEFAULT_ARG_VALUES( ... )    \
    auto args() {                    \
        return arg_t{ __VA_ARGS__ }; \
    };

#define DECLARE_ARG_NAMES( ... )               \
    string_list& arg_names() {                 \
        static string_list __v{ __VA_ARGS__ }; \
        return __v;                            \
    }

template <typename T>
struct arg_binding_t {
    arg_binding_t( const T& v_ )
        : v( v_ ) {}

    arg_binding_t& operator==( const T& v_ ) {
        v = v_;
    }

    T v;
};

//--inlines------------------------------------------------------------------------
inline bool code::operator<( const code& c_ ) const {
    return strncmp( _code, c_._code, sizeof( _code ) ) < 0;
}

inline code::code( const char* code_ )
    : _code{ 0 } {
    memcpy( _code, code_, std::min( sizeof( _code ) - 1, strlen( code_ ) ) );
}

inline char* code::c_str() const {
    return const_cast<char*>( _code );
}

inline size_t code::length() const {
    return strlen( _code );
}

inline code::code( const xstring& str_ )
    : code{ str_.c_str() } {
}

inline code::code( int c_ ) {
    sprintf( _code, "%d", c_ );
}

inline bool code::empty() const {
    return _code[ 0 ] == '\0';
}

inline code& code::operator=( const code& c_ ) {
    memcpy( _code, c_._code, sizeof( _code ) );
    return *this;
}

inline bool code::operator==( const char* c_ ) const {
    if ( length() != strlen( c_ ) ) return false;
    return *this == code( c_ );
}

inline bool code::operator!=( const char* c_ ) const {
    if ( length() != strlen( c_ ) ) return true;
    return *this != code( c_ );
}

inline bool code::operator==( const code& c_ ) const {
    return ( this == &c_ ) || strncmp( _code, c_._code, sizeof( _code ) ) == 0;
}

inline bool code::operator!=( const code& c_ ) const {
    return !( *this == c_ );
}
#if 0
inline code::operator char*() {
    return _code;
}

inline code::operator const char*() const {
    return _code;
}
#endif

#if 0
// 转成秒--时间粒度不仅仅要和绝对时间有关系还和分割边界有关系，比如1天显然不能按照绝对秒数来算，应该按照收盘时间和开盘时间来算，每周则只能基于小时来算,分钟其实会出现跨天的状况,可以按照秒来算，同时按照自然阅读来分
inline period::operator uint32_t() {
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

// todo deprecated.
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
    NVX_ASSERT( ( uint32_t )index_ < _size );
    return element( index_ );
}

template <typename T>
inline void array_t<T>::set( const T& t_, int index_ ) {
    NVX_ASSERT( ( uint32_t )index_ < _size );
    element[ index_ ] = t_;
}

template <typename T>
inline void array_t<T>::set( const T&& t_, int index_ ) {
    NVX_ASSERT( ( uint32_t )index_ < _size );
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

#endif

inline datetime datetime::now() {
    datetime t;
    t.from_unix_time( time( 0 ) );

    return t;
}

inline datetime& datetime::from_unix_time( const time_t& t_ ) {
    auto tm  = localtime( &t_ );
    d.year   = tm->tm_year + 1900;
    d.month  = tm->tm_mon + 1;
    d.day    = tm->tm_mday;
    d.wday   = tm->tm_wday;
    t.hour   = tm->tm_hour;
    t.minute = tm->tm_min;
    t.second = tm->tm_sec;
    t.milli  = 0;

    return *this;
}

inline time_t datetime::to_unix_time() const {
    struct tm t0;
    t0.tm_year = d.year - 1900;
    t0.tm_mon  = d.month - 1;
    t0.tm_mday = d.day;
    t0.tm_hour = t.hour;
    t0.tm_min  = t.minute;
    t0.tm_sec  = t.second;

    return mktime( &t0 );
}

inline bool datetime::is_valid() const {
    return d.day != 0;
}

#define TK_DIFF ( 1000 + 100 + 10 + 1 ) * '0'
#define H_DIFF ( 10 + 1 ) * '0'

inline void datetime::print( const char* prefix_, FILE* fp_ ) const {
    if ( !fp_ ) {
        fp_ = stdout;
    }

    fprintf( fp_, "%s %s\n", prefix_, to_iso().c_str() );
}

// YYYYMMDD，HH:MM:SS，MILLI ctp独有的格式
inline void datetime::from_ctp( const char* day_, const char* time_, int milli_ ) {
    // year  = ( day_[ 0 ] - '0' ) * 1000 + ( day_[ 1 ] - '0' ) * 100 + ( day_[ 2 ] - '0' ) * 10 + ( day_[ 3 ] - '0' );
    // month = ( day_[ 4 ] - '0' ) * 10 + ( day_[ 5 ] - '0' );
    // day   = ( day_[ 6 ] - '0' ) * 10 + ( day_[ 7 ] - '0' );
    d.year  = day_[ 0 ] * 1000 + day_[ 1 ] * 100 + day_[ 2 ] * 10 + day_[ 3 ] - TK_DIFF;
    d.month = day_[ 4 ] * 10 + day_[ 5 ] - H_DIFF;
    d.day   = day_[ 6 ] * 10 + day_[ 7 ] - H_DIFF;

    // hour    = ( time_[ 0 ] - '0' ) * 10 + ( time_[ 1 ] - '0' );
    // minute  = ( time_[ 3 ] - '0' ) * 10 + ( time_[ 4 ] - '0' );
    // seconds = ( time_[ 6 ] - '0' ) * 10 + ( time_[ 7 ] - '0' );
    t.hour   = time_[ 0 ] * 10 + time_[ 1 ] - H_DIFF;
    t.minute = time_[ 3 ] * 10 + time_[ 4 ] - H_DIFF;
    t.second = time_[ 6 ] * 10 + time_[ 7 ] - H_DIFF;
    t.milli  = milli_;
    // w=y+[y/4]+[c/4]-2c+[26(m+1)/10]+d-1
    // 2049: y=49, c=20,-0 星期日
    // wday = ( year % 100 + ( year % 100 ) / 4 + ( year / 100 ) / 4 - 2 * ( year / 100 ) + 26 * ( month + 1 ) / 10 + day - 1 ) % 7;
}

inline std::string datetime::to_iso() const {
    char fmt[ 64 ];
    sprintf( fmt, "%04d-%02d-%02d %02d:%02d:%02d.%03dZ", d.year, d.month, d.day, t.hour, t.minute, t.second, t.milli );

    return fmt;
}

NVX_NS_END

#endif /* C096ECC6_D3E8_4656_A4DF_F125629A8BE4 */
