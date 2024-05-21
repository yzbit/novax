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

nvx_st Portfolio::update( const order_update_t& upt_ ) {
#if 0
    PositionImpl* p = nullptr;

    code_t c = codeOf( upt_.id );

    switch ( upt_.dir ) {
    default: assert( 0 ); break;
    case odir_t::p_long:
        p = pos( c, dist_t::netlong );
        break;
    case odir_t::p_short:
        break;
    case odir_t::cover:
        break;
    case odir_t::sell:
        break;
    }

    p = upt_.dir == odir_t::p_long || upt_.dir == odir_t::sell ? pos( c, dist_t::netlong )
                                                               : pos( c, dist_t::netshort );
    if ( !p ) {
        return NVX_Fail;
    }

    if ( upt_.dir == odir_t::cover || upt_.dir == odir_t::sell ) {
        auto rem = p->herge( upt_.qty, upt_.price );
        assert( rem >= 0 );
    }
    else {
        p->accum( upt_.qty, upt_.price );
    }
#endif
    return NVX_OK;
}

PositionImpl* Portfolio::pos( const code_t& code_, dist_t d_ ) {
#if 0
    if ( auto found = std::find( _repo.begin(),
                                 _repo.end(),
                                 [ & ]( PositionImpl* p ) {
                                     return ( p->symbol() == code_ )
                                            && ( ( p->is_short() && d_ == Portfolio::dist_t::netshort )
                                                 || ( !p->is_short() && d_ == Portfolio::dist_t::netlong ) );
                                 } );
         found != _repo.end() ) {
        return *found;
    }
#endif

    return nullptr;
}

PositionImpl* Portfolio::add( const code_t& code_, dist_t d_ ) {
#if 0
    auto p = pos( code_, d_ );
    if ( p ) return p;

    auto c = _repo.size();
    for ( int i = 0; i < c; ++i ) {
        if ( !_repo[ i ] ) {
            auto newp = new PositionImpl( code_, d_ );
            newp->reset();
            _repo[ i ] = newp;

            return _repo[ i ];
        }
    }
#endif

    return nullptr;
}

Portfolio::Portfolio() {
    _repo.reserve( 128 );
}
//-----order book
order_t* OrderBook::find( oid_t id_ ) {
    auto found = _ords.find( id_ );

    return found == _ords.end() ? nullptr
                                : &found->second;
}

OrderBook::OrderBook( oid_t init_id_ )
    : _start_id( init_id_ ) {
}

OrderBook::~OrderBook() {
}

order_t* OrderBook::append() {
    auto ord = order_t();
    ord.id   = ++_start_id;
    _ords.try_emplace( ord.id, ord );
    return &_ords[ ord.id ];
}
//------------------------
OrderMgmt::~OrderMgmt() {}
OrderMgmt::OrderMgmt( IBroker* ib_, id_t id_start_ )
    : _record( id_start_ )
    , _ib( ib_ ) {}

nvx_st OrderMgmt::start() { return _ib->start(); }
nvx_st OrderMgmt::stop() { return _ib->stop(); }

oid_t OrderMgmt::put( odir_t        dir_,
                      const code_t& code_,
                      vol_t         qty_,
                      price_t       price_,
                      otype_t       mode_,
                      const text_t& remark_ ) {
#if 0
    auto r   = order_t( code_, qty_, price_, mode_, dir_ );
    r.id     = oid();
    r.remark = remark_;

    if ( 0 != _ib->put( r ) ) {
        LOG_INFO( "put order failed, delete it;oid=[%u]", r.id );
        return kBadId;
    }

    _record.emplace( r.id, r );

    LOG_INFO( "order count in book: [ %lu ]", _record.size() );
    return r.id;
#endif
    return 0;
}

nvx_st OrderMgmt::close( const code_t& code_ ) {
    NVX_ASSERT( 0 );

    return NVX_OK;
}

oid_t OrderMgmt::sellshort( const code_t& code_,
                            vol_t         qty_,
                            price_t       price_,
                            otype_t       mode_,
                            const text_t& remark_ ) {
    LOG_TAGGED( "om", "short: code=%s qty%d price=%.2f mode=%d r=%s", code_.c_str(), qty_, price_, mode_, remark_.c_str() );

    return put( odir_t::p_short, code_, qty_, price_, mode_, remark_ );
}

oid_t OrderMgmt::buylong( const code_t& code_,
                          vol_t         qty_,
                          price_t       price_,
                          otype_t       mode_,
                          const text_t& remark_ ) {
    LOG_TAGGED( "om", "long: code=%s qty%d price=%.2f sl=%d, tp=%d, r=%s", code_.c_str(), qty_, price_, ( int )mode_, remark_.c_str() );

    return put( odir_t::p_long, code_, qty_, price_, mode_, remark_ );
}

nvx_st OrderMgmt::cancel( oid_t id_ ) {
#if 0
    LOG_TAGGED( "om", "del order: %u", id_ );
    auto o = get( id_ );
    if ( !o ) {
        LOG_TAGGED( "om", "cannot find order: %u", id_ );
        return NVX_Fail;
    }

    auto& r = o.value().get();
    if ( !o
         || ( r.status != ostatus_t::pending
              && r.status != ostatus_t::partial_dealed
              && r.status != ostatus_t::patial_canelled ) ) {
        LOG_TAGGED( "om", "can not cancel order, id=%u status=%d", id_, r.status );
        return NVX_Fail;
    }

    return _ib->cancel( r );
#endif
    return NVX_OK;
}

nvx_st OrderMgmt::remove( oid_t id_ ) {
#if 0
    _record.erase( id_ );
#endif
    return NVX_OK;
}

void OrderMgmt::update_ord( const order_update_t& upt_ ) {
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

IPosition* OrderMgmt::position( const code_t& code_, bool long_ ) {
    return _pf.pos( code_,
                    long_ ? Portfolio::dist_t::netlong
                          : Portfolio::dist_t::netshort );
}

void OrderMgmt::update_position() {
}

nvx_st OrderMgmt::sell( const code_t& code_,
                        const vol_t   qty_,
                        const price_t price_,
                        const otype_t mode_,
                        const text_t& remark_ ) {
    LOG_TAGGED( "om", "sell: code=%s qty=%d price=%.2f, mode=%d, r=%s", code_.c_str(), qty_, price_, ( int )mode_, remark_.c_str() );

#if 0
    order_t r( code_, qty_, price_, mode_, odir_t::sell );
    r.remark = remark_;
    r.id     = oid();

    return close( r );
#endif
    return NVX_OK;
}

nvx_st OrderMgmt::buy( const code_t& code_,
                       const vol_t   qty_,
                       const price_t price_,
                       const otype_t mode_,
                       const text_t& remark_ ) {

    LOG_TAGGED( "om", "buy: code=%s qty=%d price=%.2f, mode=%d, r=%s", code_.c_str(), qty_, price_, ( int )mode_, remark_.c_str() );
#if 0
    order_t r( code_, qty_, price_, mode_, odir_t::cover );

    r.id = oid();

    return close( r );
#endif

    return NVX_OK;
}

nvx_st OrderMgmt::close( const order_t& r_ ) {
    NVX_ASSERT( r_.dir == odir_t::sell || r_.dir == odir_t::cover );

    if ( r_.qty == 0 ) {
        LOG_INFO( "close [%s] with qty=0, !!will close all avaiable", r_.code.c_str() );
    }

    auto pv = r_.dir == odir_t::sell
                  ? long_position( r_.code.c_str() )
                  : short_position( r_.code.c_str() );

    if ( 0 >= pv ) {
        LOG_INFO( "no position of [%s] , close ign", r_.code.c_str() );
        return NVX_Fail;
    }

    if ( pv > r_.qty && r_.qty > 0 ) pv = r_.qty;

    LOG_INFO( "close position for [%s %d]", r_.code.c_str(), pv );

    if ( 0 != _ib->put( r_ ) ) {
        LOG_INFO( "close position failed,id=%u ,sym=%s", r_.id, r_.code.c_str() );
        return NVX_Fail;
    }

    // todo
    //_record.emplace( order.code, order );

    return NVX_OK;
}

nvx_st OrderMgmt::close( oid_t id_ ) {
    return NVX_OK;
}

NVX_NS_END