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
    fund,
    position,
    error
};

using order_msg_t = int;
using error_msg_t = int;
using pos_msg_t   = int;
using tick_msg_t  = quotation_t;
using fund_msg_t  = fund_t;

#define DECLARE_MSG( ... )              \
    msg_type _type = msg_type::unknown; \
    char     _data[ MaxTypeSize<__VA_ARGS__>::value ]

struct msg_t {
    DECLARE_MSG( tick_msg_t, fund_msg_t );

    msg_type type() const { return _type; }
    msg_t();
    msg_t& operator=( const msg_t& oth_ );

    template <typename T, typename U = std::remove_cv_t<std::remove_reference_t<T>>>
    msg_t( const T& t_ );

    template <typename T>
    const T& get() const;
};

//-----impl
inline msg_t::msg_t() {
    memset( _data, 0x00, sizeof( _data ) );
    _type = msg_type::unknown;
}

inline msg_t& msg_t::operator=( const msg_t& oth_ ) {
    _type = oth_._type;
    memcpy( _data, oth_._data, sizeof( _data ) );
    return *this;
}

template <typename T, typename U>
inline msg_t::msg_t( const T& t_ ) {
    memset( _data, 0x00, sizeof( _data ) );

    if constexpr ( std::is_same_v<U, tick_msg_t> ) {
        _type = msg_type::tick;
    }
    else if constexpr ( std::is_same_v<U, fund_msg_t> ) {
        _type = msg_type::fund;
    }
    else {
        _type = msg_type::unknown;
    }

    auto len = std::min( sizeof( _data ), sizeof( U ) );
    memcpy( _data, &t_, len );
}

template <typename T>
inline const T& msg_t::get() const {
    return *reinterpret_cast<const T*>( _data );
}

}  // namespace pub

struct IPub {
    virtual ~IPub() {}
    virtual int post( const pub::msg_t& m_ ) = 0;
};

#define PUB( m ) post( m )

NVX_NS_END

#endif /* C3C3FFA3_AD73_4439_8415_28F7FB647AAC */
