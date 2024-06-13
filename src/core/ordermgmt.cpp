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

#include <memory>
#include <novax.h>

#include "ordermgmt.h"

#include "context.h"
#include "ctp/tradeproxy.h"
#include "log.hpp"
#include "proxy.h"
#include "quant.h"

NVX_NS_BEGIN

nvx_st portfolio::update( const order_update& upt_ ) {
#if 0
    position_impl* p = nullptr;

    code c = codeOf( upt_.id );

    switch ( upt_.dir ) {
    default: assert( 0 ); break;
    case ord_dir::p_long:
        p = pos( c, dist_t::netlong );
        break;
    case ord_dir::p_short:
        break;
    case ord_dir::cover:
        break;
    case ord_dir::sell:
        break;
    }

    p = upt_.dir == ord_dir::p_long || upt_.dir == ord_dir::sell ? pos( c, dist_t::netlong )
                                                               : pos( c, dist_t::netshort );
    if ( !p ) {
        return NVX_FAIL;
    }

    if ( upt_.dir == ord_dir::cover || upt_.dir == ord_dir::sell ) {
        auto rem = p->herge( upt_.qty, upt_.price );
        assert( rem >= 0 );
    }
    else {
        p->accum( upt_.qty, upt_.price );
    }
#endif
    return NVX_OK;
}

position_impl* portfolio::pos( const code& code_, dist_t d_ ) {
#if 0
    if ( auto found = std::find( _repo.begin(),
                                 _repo.end(),
                                 [ & ]( position_impl* p ) {
                                     return ( p->symbol() == code_ )
                                            && ( ( p->is_short() && d_ == portfolio::dist_t::netshort )
                                                 || ( !p->is_short() && d_ == portfolio::dist_t::netlong ) );
                                 } );
         found != _repo.end() ) {
        return *found;
    }
#endif

    return nullptr;
}

position_impl* portfolio::add( const code& code_, dist_t d_ ) {
#if 0
    auto p = pos( code_, d_ );
    if ( p ) return p;

    auto c = _repo.size();
    for ( int i = 0; i < c; ++i ) {
        if ( !_repo[ i ] ) {
            auto newp = new position_impl( code_, d_ );
            newp->reset();
            _repo[ i ] = newp;

            return _repo[ i ];
        }
    }
#endif

    return nullptr;
}

portfolio::portfolio() {
    _repo.reserve( 128 );
}

//-----order book
#if 0
order* ord_book::find( oid id_ ) {
    auto found = _ords.find( id_ );

    return found == _ords.end() ? nullptr
                                : &found->second;
}

ord_book::ord_book() {
}

ord_book::~ord_book() {
}

order* ord_book::append( order& order_ ) {
    order_.id = oid();
    _ords.try_emplace( order_.id, order_ );
    return &_ords[ order_.id ];
}

size_t ord_book::count() const {
    return _ords.size();
}
#endif

//------------------------
ord_mgmt::~ord_mgmt() {}
ord_mgmt::ord_mgmt() {}

void ord_mgmt::update( const order_update& upt_ ) {
#if 0
    auto o = get( upt_.id );
    if ( !o ) return;

    LOG_INFO( "upt status %d", upt_.status );
    // NVX_ASSERT( status_ != ord_status::dealt );

    if ( ord_status::cancelled == status_ ) {
        LOG_INFO( "close order: id=%u ", upt_.id );
        remove( upt_.id );
    }

    // r.traded += o_.traded;

    if ( r.traded == r.qty ) {
        LOG_INFO( "order of [ %u ] complete dealt, eraise it", r.id );
        remove( upt_.id );
    }

    if ( NVX_OK != _pf.update( upt_ ) ) {
        LOG_INFO( "bad portfolio updating" );
    }
#endif
}

position* ord_mgmt::pos_of( const code& code_, bool long_ ) {
    return _pf.pos( code_,
                    long_ ? portfolio::dist_t::netlong
                          : portfolio::dist_t::netshort );
}

void ord_mgmt::update_position() {
}

nvx_st ord_mgmt::add( oid id_, const code& c_, vol qty_, price limit_, ord_dir dir_, price stop_, stop_dir sdir_, ord_type type_, const text& remark_ ) {
    LOG_INFO( "insert new ord: id=%u,code=%s qty=%f, price=%f", id_, c_.c_str(), qty_, limit_ );
    order ord = {
        .id     = id_,
        .symbol = c_,
        .limit  = limit_,
        .stop   = stop_,
        .qty    = qty_,
        .dir    = dir_,
        .sdir   = sdir_,
        // .type   = type_,
        .status = ord_status::pending,
        .remark = remark_,
    };

    auto res = _orders.try_emplace( id_, ord );

    if ( !res.second ) {
        LOG_INFO( "add order faild, id=%u code=%s", id_, c_.c_str() );
        return NVX_FAIL;
    }

    return NVX_OK;
}

order* ord_mgmt::find( oid id_ ) {
    LOG_TAGGED( "om", "del order: %u", id_ );
    auto res = _orders.find( id_ );

    if ( res == _orders.end() ) {
        LOG_TAGGED( "om", "cannot find order: %u", id_ );
        return nullptr;
    }

    return &( res->second );
}

NVX_NS_END