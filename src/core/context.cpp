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

#include <cassert>
#include <list>

#include "context.h"

#include "aspect.h"
#include "data.h"
#include "log.hpp"
#include "order_mgmt.h"
#include "quant.h"
#include "strategy.h"

NVX_NS_BEGIN

#define TRADER ( *dynamic_cast<OrderMgmt*>( QUANT.trader() ) )

Context::Context() {}

const quotation_t& Context::qut() const {
    return _qut;
}

const fund_t Context::fund() const {
    return _fund;
}

void Context::update_qut( const quotation_t& q_ ) {
    _qut = q_;
    QUANT.strategy()->prefight( this );
}

void Context::update_fund( const fund_t& f_ ) {
    _fund = f_;
}

Aspect* Context::load( const code_t& symbol_, const period_t& period_, int count_ ) {
    // return ASP.add( symbol_, period_, count_ );
    // todo
    return nullptr;
}

oid_t Context::open( const code_t& c_, vol_t qty_, price_t sl_, price_t tp_, price_t price_, otype_t mode_ ) {
    if ( qty_ < 0 )
        return TRADER.sellshort( c_, -qty_, price_, mode_, sl_, tp_ );
    else if ( qty_ > 0 )
        return TRADER.buylong( c_, qty_, price_, mode_ );
    else {
        assert( false );
        return 0;
    };

    return 0;
}

nvx_st Context::close( const code_t& c_, vol_t qty_, price_t price_, otype_t mode_ ) {
    if ( qty_ < 0 ) {
        return TRADER.buy( c_, -qty_, price_, mode_ );
    }
    else if ( qty_ > 0 ) {
        return TRADER.sell( c_, qty_, price_, mode_ );
    }
    else {
        return TRADER.close( c_ );
    }
}

vol_t Context::position() const {
#if 0
    auto& pf = p();
    vol_t v  = 0;
    pf.for_each( []( position_t& p ) {
        v += p.position;
    } );

    return v;
    return p().dealt();

#endif
    return vol_t();
}

vol_t Context::position( const code_t& c_ ) const {
    return 0;
}

vol_t Context::pending() const {
    return 0;
}

vol_t Context::pending( const code_t& c_ ) const {
    return 0;
}

price_t Context::put_price() const {
    return 0;
}

price_t Context::last_deal() const {
    return 0;
}

kidx_t Context::last_entry() const {
    return 0;
}

kidx_t Context::last_exit() const {
    return 0;
}

NVX_NS_END