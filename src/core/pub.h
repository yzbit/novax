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

#ifndef C3C3FFA3_AD73_4439_8415_28F7FB647AAC
#define C3C3FFA3_AD73_4439_8415_28F7FB647AAC
#include "models.h"
#include "ns.h"
#include "utils.hpp"

NVX_NS_BEGIN

namespace pub {
enum class msg_type {
    unknown,
    tick,
    order,
    acct,
    position,
    error,
    timer
};

using timer_msg = int;
using order_msg = order_update;
using error_msg = nvxerr_t;
using pos_msg   = int;
using tick_msg  = tick;
using acct_msg  = funds;

#define DECLARE_MSG( ... )              \
    msg_type _type = msg_type::unknown; \
    char     _data[ MaxTypeSize<__VA_ARGS__>::value ]

struct msg {
    DECLARE_MSG( tick_msg, acct_msg );

    msg_type type() const { return _type; }
    msg();
    msg& operator=( const msg& oth_ );

    template <typename T, typename U = std::remove_cv_t<std::remove_reference_t<T>>>
    msg( const T& t_ );

    template <typename T>
    const T& get() const;
};

//-----impl
inline msg::msg() {
    memset( _data, 0x00, sizeof( _data ) );
    _type = msg_type::unknown;
}

inline msg& msg::operator=( const msg& oth_ ) {
    _type = oth_._type;
    memcpy( _data, oth_._data, sizeof( _data ) );
    return *this;
}

template <typename T, typename U>
inline msg::msg( const T& t_ ) {
    memset( _data, 0x00, sizeof( _data ) );

    if constexpr ( std::is_same_v<U, tick_msg> ) {
        _type = msg_type::tick;
    }
    else if constexpr ( std::is_same_v<U, acct_msg> ) {
        _type = msg_type::acct;
    }
    else {
        _type = msg_type::unknown;
    }

    auto len = std::min( sizeof( _data ), sizeof( U ) );
    memcpy( _data, &t_, len );
}

template <typename T>
inline const T& msg::get() const {
    return *reinterpret_cast<const T*>( _data );
}

}  // namespace pub

struct ipub {
    virtual ~ipub() {}
    virtual nvx_st post( const pub::msg& m_ ) = 0;
};

#define PUB( m ) post( m )

NVX_NS_END

#endif /* C3C3FFA3_AD73_4439_8415_28F7FB647AAC */
