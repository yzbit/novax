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
#include <definitions.h>
#include <map>
#include <mutex>
#include <optional>
#include <stdlib.h>
#include <string.h>
#include <type_traits>

#include "../ns.h"
#include "../utils.hpp"

NVX_NS_BEGIN

namespace ctp {

template <typename T, typename = std::enable_if_t<std::is_array_v<T>>>
struct Encaps {
    enum { LENGTH = sizeof( T ) };

    Encaps() = default;
    Encaps( const T& ex_ ) {
        memcpy( _data, &ex_, LENGTH );
        _data[ LENGTH ] = 0;
    }

    Encaps( const Encaps& ex_ ) {
        memcpy( _data, &ex_._data, sizeof( _data ) );
    }

    char* data() {
        return _data;
    }

    size_t length() const {
        return LENGTH;
    }

    void copy_to( T& ref_ ) {
        memcpy( ref_, _data, LENGTH );
    }

    bool operator==( const Encaps& r_ ) {
        return ( this == &r_ ) || ( 0 == memcmp( _data, r_._data, LENGTH ) );
    }

    bool operator==( const T& r_ ) {
        return 0 == memcmp( _data, r_, LENGTH );
    }

    bool operator!=( const Encaps& r_ ) {
        return ( this != &r_ ) && ( 0 != memcmp( _data, r_._data, LENGTH ) );
    }

    bool operator!=( const T& r_ ) {
        return 0 != memcmp( _data, r_, LENGTH );
    }

    Encaps& operator=( const Encaps& r_ ) {
        if ( this == &r_ ) return *this;

        memcpy( _data, r_._data, sizeof( _data ) );
        return *this;
    }

    Encaps& operator=( const T& r_ ) {
        memcpy( _data, r_, Encaps<T>::LENGTH );
        _data[ Encaps<T>::LENGTH ] = 0;

        return *this;
    }

    bool is_valid() {
        return !!_data[ 0 ];
    }

protected:
    char _data[ LENGTH + 1 ] = { 0 };
};

struct ref_t : Encaps<TThostFtdcOrderRefType> {
    explicit ref_t( const TThostFtdcOrderRefType& r_ )
        : Encaps( r_ ) {}

    explicit ref_t( unsigned ref_ ) {
        from( ref_ );
    }

    explicit ref_t( const ref_t& r_ )
        : Encaps( r_ ) {}

    ref_t() {
        memset( _data, '0', LENGTH );
    }

    ref_t& operator=( int r_ ) {
        from( r_ );
        return *this;
    }

    ref_t& operator+=( int diff_ ) {
        from( int_val() + diff_ );
        return *this;
    }

    ref_t& operator++() {
        from( int_val() + 1 );
        return *this;
    }

    ref_t operator+( int diff_ ) {
        return ref_t( int_val() + diff_ );
    }

    bool operator==( unsigned ref_ ) {
        return int_val() == ref_;
    }

    bool operator==( const ref_t& ref_ ) {
        return Encaps::operator==( ref_ );
    }

    unsigned int_val() const {
        return std::atoi( _data );
    }

private:
    void from( unsigned v_ ) {
        memset( _data, '0', sizeof( _data ) );
        int i        = LENGTH;
        _data[ i-- ] = 0;
        while ( v_ && i >= 0 ) {
            _data[ i-- ] = v_ % 10 + '0';
            v_ /= 10;
        }
    }
};

struct sess_t {
    int front = 0;
    int conn  = 0;

    bool operator==( const sess_t& s_ ) {
        return front == s_.front && conn == s_.conn;
    }
};

using ctpex_t = Encaps<TThostFtdcExchangeIDType>;
using exoid_t = Encaps<TThostFtdcOrderSysIDType>;

struct fsr_t {
    sess_t ss;
    ref_t  ref;

    fsr_t() = default;
    fsr_t( int f_, int s_, const ref_t& r_ )
        : ss( { f_, s_ } )
        , ref( r_ ) {}

    bool operator==( const fsr_t& r_ ) {
        return ( this == &r_ ) || ( ss == r_.ss && ref == r_.ref );
    }
};

struct order_id_t {
    fsr_t   fsr;
    ctpex_t ex;
    exoid_t id;

    order_id_t() = default;
    bool has_sysid() {
        return id.is_valid();
    }

    bool has_refid() {
        return fsr.ref.is_valid();
    }

    bool is_valid() {
        return has_refid() || has_sysid();
    }
};

struct IdMgr {
    using opt_id = std::optional<order_id_t>;

    nvx_st insert( const order_id_t& id_ ) {
        std::unique_lock<Spinner> lck{ _sp };

        return _ids.try_emplace( id_.fsr.ref.int_val(), id_ ).second
                   ? NVX_OK
                   : NVX_Fail;
    }

    nvx_st update_sysid( const fsr_t& fsr_, const ctpex_t& ex_, const exoid_t& oid_ ) {
        std::unique_lock<Spinner> lck{ _sp };

        for ( auto& [ k, v ] : _ids ) {
            if ( v.fsr == fsr_ ) {
                v.ex = ex_;
                v.id = oid_;
                return NVX_OK;
            }
        }

        return NVX_Fail;
    }

    void remove( const oid_t& id_ ) {
        std::unique_lock<Spinner> lck{ _sp };
        _ids.erase( id_ );
    }

    void remove( const fsr_t& ref_ ) {
        std::unique_lock<Spinner> lck{ _sp };
        _ids.erase( std::find_if( _ids.begin(), _ids.end(), [ & ]( auto& pair ) { return pair.second.fsr == ref_; } ) );
    }

    opt_id id_of( const ctpex_t& ex_, const exoid_t& oid_ ) {
        std::unique_lock<Spinner> lck{ _sp };
        return id_of( std::find_if( _ids.begin(), _ids.end(), [ & ]( auto& pair ) { return pair.second.id == oid_ && pair.second.ex == ex_; } ) );
    }

    opt_id id_of( const fsr_t& ref_ ) {
        std::unique_lock<Spinner> lck{ _sp };
        return id_of( std::find_if( _ids.begin(), _ids.end(), [ & ]( auto& pair ) { return pair.second.fsr == ref_; } ) );
    }

    opt_id id_of( const oid_t& id_ ) {
        std::unique_lock<Spinner> lck{ _sp };
        return id_of( _ids.find( id_ ) );
    }

private:
    using IdMap = std::map<oid_t, order_id_t>;

    opt_id id_of( IdMap::iterator itr_ ) {
        return itr_ != _ids.end() ? opt_id( itr_->second )
                                  : std::nullopt;
    };

private:
    Spinner _sp;
    IdMap   _ids;
};
};  // namespace ctp

NVX_NS_END

#endif /* EBC2EDD7_C2AC_4519_9357_CE387B50513F */
