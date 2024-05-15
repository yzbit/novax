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

#ifndef EBC2EDD7_C2AC_4519_9357_CE387B50513F
#define EBC2EDD7_C2AC_4519_9357_CE387B50513F

#include <ctp/ThostFtdcTraderApi.h>
#include <stdlib.h>
#include <string.h>

#include "../ns.h"

NVX_NS_BEGIN
namespace ctp {
struct order_ref_t {
    explicit order_ref_t( const TThostFtdcOrderRefType& ctp_ref_ );
    // explicit order_ref_t( const char* sz_ref_ );
    explicit order_ref_t( unsigned ref_ );
    explicit order_ref_t( const order_ref_t& r_ );
    order_ref_t();

    void         copy( TThostFtdcOrderRefType& ref_ );
    order_ref_t& operator=( const order_ref_t& r_ );
    order_ref_t& operator=( int r_ );
    order_ref_t& operator+=( int diff_ );
    order_ref_t& operator++();
    order_ref_t  operator+( int diff_ );
    bool         operator==( const order_ref_t& );
    bool         operator==( unsigned ref_ );
    bool         operator==( const TThostFtdcOrderRefType& ref_ );
    const char*  str_val() const;
    unsigned     int_val() const;

private:
    void from( unsigned v_ );

private:
    enum {
        DATA_LENGTH = sizeof( TThostFtdcOrderRefType ) + 1
    };
    char _data[ DATA_LENGTH ];
};

struct session_t {
    int         front;
    int         sess;
    order_ref_t init_ref;

    session_t() = default;
    session_t( TThostFtdcFrontIDType f_, TThostFtdcSessionIDType s_, const TThostFtdcOrderRefType& r_ )
        : front( f_ )
        , sess( s_ )
        , init_ref( r_ ) {}
};

struct sys_order_t {
    char ex_id[ 9 ];
    char order_id[ 21 ];

    sys_order_t( const sys_order_t& oth_ );
    sys_order_t();
    sys_order_t( const TThostFtdcExchangeIDType& ex_, const TThostFtdcOrderSysIDType& oid_ );

    bool         is_valid() const;
    sys_order_t& operator=( const sys_order_t& oth_ );
    bool         operator==( const sys_order_t& eoid_ ) const;
    bool         operator!=( const sys_order_t& eoid_ ) const;
};

struct fsr_t {
    int         front;
    int         sess;
    order_ref_t ref;

    fsr_t() = default;
    fsr_t( int f_, int s_, const order_ref_t& r_ )
        : front( f_ )
        , sess( s_ )
        , ref( r_ ) {}

    bool operator==( const fsr_t& r_ ) {
        return this == &r_ || ( front == r_.front && sess == r_.sess && ref == r_.ref );
    }
};

struct order_id_t {
    fsr_t       fsr;
    sys_order_t sysid;

    order_id_t() = default;
    bool has_sysid() {
        return !!sysid.order_id[ 0 ];
    }
};

//--------------------------impl------------------------------------
inline bool order_ref_t::operator==( const order_ref_t& r_ ) {
    return memcmp( _data, r_._data, sizeof( _data ) ) == 0;
}

inline bool order_ref_t::operator==( unsigned ref_ ) {
    return int_val() == ref_;
}

inline bool order_ref_t::operator==( const TThostFtdcOrderRefType& ref_ ) {
    return memcmp( _data, ref_, sizeof( TThostFtdcOrderRefType ) ) == 0;
}

inline order_ref_t::order_ref_t( const TThostFtdcOrderRefType& ctp_ref_ ) {
    memcpy( _data, ctp_ref_, sizeof( TThostFtdcOrderRefType ) );
    _data[ DATA_LENGTH - 1 ] = 0;
}

// order_ref_t::order_ref_t( const char* sz_ref_ )
//     : order_ref_t( atoi( sz_ref_ ) ) {
// }

inline void order_ref_t::from( unsigned v_ ) {
    // todo
    memset( _data, '0', sizeof( _data ) );

    int i = DATA_LENGTH - 1;

    _data[ i-- ] = 0;
    while ( v_ && i >= 0 ) {
        _data[ i-- ] = v_ % 10 + '0';
        v_ /= 10;
    }
}

inline order_ref_t::order_ref_t( unsigned ref_ ) {
    from( ref_ );
}

inline order_ref_t::order_ref_t( const order_ref_t& r_ )
    : order_ref_t( r_.int_val() ) {}

inline order_ref_t::order_ref_t() {
    memset( _data, '0', DATA_LENGTH );
    _data[ DATA_LENGTH - 1 ] = 0;
}

inline order_ref_t& order_ref_t::operator++() {
    from( int_val() + 1 );
    return *this;
}

inline order_ref_t& order_ref_t::operator=( const order_ref_t& r_ ) {
    memcpy( _data, r_._data, sizeof( _data ) );
    return *this;
}

inline order_ref_t& order_ref_t::operator=( int r_ ) {
    from( r_ );
    return *this;
}

inline order_ref_t& order_ref_t::operator+=( int diff_ ) {
    from( int_val() + diff_ );
    return *this;
}

inline order_ref_t order_ref_t::operator+( int diff_ ) {
    return order_ref_t( int_val() + diff_ );
}

inline const char* order_ref_t::str_val() const {
    return _data;
}

inline unsigned order_ref_t::int_val() const {
    return std::atoi( _data );
}

inline void order_ref_t::copy( TThostFtdcOrderRefType& ref_ ) {
    memcpy( ref_, _data, sizeof( TThostFtdcOrderRefType ) );
}

//-----sysorder t
inline sys_order_t::sys_order_t( const sys_order_t& oth_ ) {
    memcpy( ex_id, oth_.ex_id, sizeof( ex_id ) );
    memcpy( order_id, oth_.order_id, sizeof( order_id ) );
}

inline sys_order_t::sys_order_t() {
    memset( ex_id, 0x00, sizeof( ex_id ) );
    memset( order_id, 0x00, sizeof( order_id ) );
}

inline sys_order_t::sys_order_t( const TThostFtdcExchangeIDType& ex_, const TThostFtdcOrderSysIDType& oid_ ) {
    memcpy( ex_id, ex_, sizeof( ex_id ) );
    memcpy( order_id, oid_, sizeof( order_id ) );
}

inline bool sys_order_t::is_valid() const {
    return ex_id[ 0 ] && order_id[ 0 ];
}

inline sys_order_t& sys_order_t::operator=( const sys_order_t& oth_ ) {
    memcpy( ex_id, oth_.ex_id, sizeof( ex_id ) );
    memcpy( order_id, oth_.order_id, sizeof( order_id ) );

    return *this;
}

inline bool sys_order_t::operator!=( const sys_order_t& oth_ ) const {
    return !( *this == oth_ );
}

inline bool sys_order_t::operator==( const sys_order_t& oth_ ) const {
    return this == &oth_ || ( memcmp( ex_id, oth_.ex_id, sizeof( ex_id ) ) == 0 && memcmp( order_id, oth_.order_id, sizeof( order_id ) ) );
}
};  // namespace ctp

NVX_NS_END

#endif /* EBC2EDD7_C2AC_4519_9357_CE387B50513F */
