#ifndef C096ECC6_D3E8_4656_A4DF_F125629A8BE4
#define C096ECC6_D3E8_4656_A4DF_F125629A8BE4

#include <algorithm>
#include <cub_ns.h>
#include <stdint.h>
#include <string.h>
#include <string>

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

#define _( _literal_ ) std::string( _literal_ )

#define THREAD_DETACHED( _func_ ) std::thread( _func_ ).detach()
#define THREAD_JOINED( _func_ ) std::thread( _func_ ).join()

CUB_NS_END

#endif /* C096ECC6_D3E8_4656_A4DF_F125629A8BE4 */
