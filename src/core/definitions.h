#ifndef C096ECC6_D3E8_4656_A4DF_F125629A8BE4
#define C096ECC6_D3E8_4656_A4DF_F125629A8BE4

#include <algorithm>
#include <array>
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
using vol_t    = double;
using oid_t    = int32_t;
using text_t   = std::string;
using string_t = std::string;
using money_t  = double;

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

    void from( const time_t& t_ ) {
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

    // YYYYMMDD，HH:MM:SS，MILLI ctp独有的格式
    void from_ctp( const char* day_, const char* time_, int milli_ ) {
        year  = ( day_[ 0 ] - '0' ) * 1000 + ( day_[ 1 ] - '0' ) * 100 + ( day_[ 2 ] - '0' ) * 10 + ( day_[ 3 ] - '0' );
        month = ( day_[ 4 ] - '0' ) * 10 + ( day_[ 5 ] - '0' );
        day   = ( day_[ 6 ] - '0' ) * 10 + ( day_[ 7 ] - '0' );

        hour    = ( time_[ 0 ] - '0' ) * 10 + ( time_[ 1 ] - '0' );
        minute  = ( time_[ 3 ] - '0' ) * 10 + ( time_[ 4 ] - '0' );
        seconds = ( time_[ 6 ] - '0' ) * 10 + ( time_[ 7 ] - '0' );

        milli = milli_;
        // w=y+[y/4]+[c/4]-2c+[26(m+1)/10]+d-1
        // 2049: y=49, c=20,-0 星期日
        wday = ( year % 100 + ( year % 100 ) / 4 + ( year / 100 ) / 4 - 2 * ( year / 100 ) + 26 * ( month + 1 ) / 10 + day - 1 ) % 7;
    }

    time_t to_time_t() {
        return 0;
    }

    std::string to_iso() {
        char fmt[ 64 ];
        sprintf( fmt, "%04d-%02d-%02d %02d:%02d:%02d.%03dZ", year, month, day, hour, minute, seconds, milli );

        return fmt;
    }
};

using ex_t = code_t;  // 交易所

struct period_t {
    enum class type_t {
        mili,
        seconds,
        min,
        hour,
        day,
        week,
        year
    };

    period_t( const type_t& t_, int r_ )
        : t( t_ )
        , rep( r_ ) {
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

CUB_NS_END

#endif /* C096ECC6_D3E8_4656_A4DF_F125629A8BE4 */
