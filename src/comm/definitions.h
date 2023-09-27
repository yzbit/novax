#ifndef C096ECC6_D3E8_4656_A4DF_F125629A8BE4
#define C096ECC6_D3E8_4656_A4DF_F125629A8BE4

#include <algorithm>
#include <stdint.h>
#include <string.h>
#include <string>

#include "ns.h"

CUB_NS_BEGIN

using kid_t   = uint32_t;
using price_t = double;
using amnt_t  = double;
using oid_t   = int32_t;
using text_t  = std::string;

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

    operator char*() {
        return code;
    }

    char code[ kMaxCodeLength ] = { 0 };
};

struct period_t {
    enum class type_t {
        tick,
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
