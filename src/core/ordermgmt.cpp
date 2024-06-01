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

    return nullptr;
}

position_impl* portfolio::add( const code& code_, dist_t d_ ) {
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

    return nullptr;
}

portfolio::portfolio() {
    _repo.reserve( 128 );
}

//-----order book
order* ordbook::find( oid id_ ) {
    auto found = _ords.find( id_ );

    return found == _ords.end() ? nullptr
                                : &found->second;
}

ordbook::ordbook( oid init_id_ )
    : _start_id( init_id_ ) {
}

ordbook::~ordbook() {
}

order* ordbook::append( order& order_ ) {
    order_.id = oid();
    _ords.try_emplace( order_.id, order_ );
    return &_ords[ order_.id ];
}

size_t ordbook::count() const {
    return _ords.size();
}

//------------------------
order_mgmt::~order_mgmt() {}
order_mgmt::order_mgmt( broker* ib_, id_t id_start_ )
    : _orders( id_start_ )
    , _ib( ib_ ) {}

nvx_st order_mgmt::start() { return _ib->start(); }
nvx_st order_mgmt::stop() { return _ib->stop(); }

oid order_mgmt::put( ord_dir     dir_,
                     const code& code_,
                     vol         qty_,
                     price       price_,
                     otype       mode_,
                     const text& remark_ ) {
    auto r   = order( code_, qty_, price_, mode_, dir_ );
    r.remark = remark_;

    if ( 0 != _ib->put( r ) ) {
        LOG_INFO( "put order failed, delete it;oid=[%u]", r.id );
        return NVX_BAD_OID;
    }

    _orders.append( r );

    LOG_INFO( "order count in book: [ %lu ]", _orders.count() );
    return r.id;
}

oid order_mgmt::close( const code& code_ ) {
    NVX_ASSERT( 0 );

    return NVX_OK;
}

oid order_mgmt::sellshort( const code& code_,
                           vol         qty_,
                           price       price_,
                           otype       mode_,
                           const text& remark_ ) {
    LOG_TAGGED( "om", "short: code=%s qty%d price=%.2f mode=%d r=%s", code_.c_str(), qty_, price_, mode_, remark_.c_str() );

    return put( ord_dir::p_short, code_, qty_, price_, mode_, remark_ );
}

oid order_mgmt::buylong( const code& code_,
                         vol         qty_,
                         price       price_,
                         otype       mode_,
                         const text& remark_ ) {
    LOG_TAGGED( "om", "long: code=%s qty%d price=%.2f sl=%d, tp=%d, r=%s", code_.c_str(), qty_, price_, ( int )mode_, remark_.c_str() );

    return put( ord_dir::p_long, code_, qty_, price_, mode_, remark_ );
}

oid order_mgmt::cancel( oid id_ ) {
    LOG_TAGGED( "om", "del order: %u", id_ );
    auto r = _orders.find( id_ );

    if ( !r ) {
        LOG_TAGGED( "om", "cannot find order: %u", id_ );
        return NVX_FAIL;
    }

    if ( r->status != ostatus_t::pending
         && r->status != ostatus_t::partial_dealed
         && r->status != ostatus_t::patial_canelled ) {
        LOG_TAGGED( "om", "can not cancel order, id=%u status=%d", id_, r->status );
        return NVX_FAIL;
    }

    return _ib->cancel( *r );
}

nvx_st order_mgmt::remove( oid id_ ) {
#if 0
    _orders.erase( id_ );
#endif
    return NVX_OK;
}

void order_mgmt::update_ord( const order_update& upt_ ) {
#if 0
    auto o = get( upt_.id );
    if ( !o ) return;

    LOG_INFO( "upt status %d", upt_.status );
    // NVX_ASSERT( status_ != ostatus_t::dealt );

    if ( ostatus_t::cancelled == status_ ) {
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

position* order_mgmt::position( const code& code_, bool long_ ) {
    return _pf.pos( code_,
                    long_ ? portfolio::dist_t::netlong
                          : portfolio::dist_t::netshort );
}

void order_mgmt::update_position() {
}

oid order_mgmt::sell( const code& code_,
                      const vol   qty_,
                      const price price_,
                      const otype mode_,
                      const text& remark_ ) {
    LOG_TAGGED( "om", "sell: code=%s qty=%d price=%.2f, mode=%d, r=%s", code_.c_str(), qty_, price_, ( int )mode_, remark_.c_str() );

    auto p = _pf.pos( code_, portfolio::dist_t::netlong );
    if ( !p || 0 == p->qty ) {
        LOG_TAGGED( "om", "no long position to be closed" );
        return NVX_BAD_OID;
    }

    vol   q = p->qty >= qty_ ? qty_ : p->qty;
    order r( _orders.oid(), code_, q, price_, mode_, ord_dir::cover );

    if ( NVX_OK != _ib->put( r ) ) {
        LOG_INFO( "close position failed,id=%u ,sym=%s", r_.id, r_.code.c_str() );
        return NVX_BAD_OID;
    }

    _orders.append( r );

    return r.id;
}

oid order_mgmt::buy( const code& code_,
                     const vol   qty_,
                     const price price_,
                     const otype mode_,
                     const text& remark_ ) {

    LOG_TAGGED( "om", "buy: code=%s qty=%d price=%.2f, mode=%d, r=%s", code_.c_str(), qty_, price_, ( int )mode_, remark_.c_str() );

    auto p = _pf.pos( code_, portfolio::dist_t::netshort );
    if ( !p || 0 == p->qty ) {
        LOG_TAGGED( "om", "no short position to be closed" );
        return NVX_BAD_OID;
    }

    // p->close( qty_, price_, mode_ );

    close( p, qty_, price_, mode_ );

    vol q = p->qty >= qty_ ? qty_ : p->qty;

    order r( _orders.oid(), code_, q, price_, mode_, ord_dir::cover );

    if ( NVX_OK != _ib->put( r ) ) {
        LOG_INFO( "close position failed,id=%u ,sym=%s", r_.id, r_.code.c_str() );
        return NVX_BAD_OID;
    }

    _orders.append( r );

    return r.id;
}

// stop loss--可能同时持有空单和多单看,价格分别是p1和p2,而  p2 > price_ > p1
oid order_mgmt::stop( const code& code_, vol qty_, price price_ ) {
    auto pos = position( code_, true );
    if ( !pos ) {
    }

    return NVX_BAD_OID;
}

// take profit
oid gain( const code& code_, vol qty_, price price_ ) {

    return NVX_BAD_OID;
}

NVX_NS_END