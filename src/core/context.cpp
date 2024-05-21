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
#include <novax.h>

#include "aspect.h"
#include "contextimpl.h"
#include "data.h"
#include "log.hpp"
#include "ordermgmt.h"
#include "quant.h"
#include "strategy.h"

NVX_NS_BEGIN

ContextIntf::ContextIntf( OrderMgmt* m_ )
    : _mgmt( m_ ) {}

void ContextIntf::update_qut( const quotation_t& qut_ ) {
    _impl.qut = qut_;
}

void ContextIntf::update_fund( const fund_t& fund_ ) {
    _impl.fund = fund_;
}

void ContextIntf::update_error( const nvxerr_t& err_ ) {
    _impl.error = err_;
}

nvxerr_t ContextIntf::error() const {
    return _impl.error;
}

const quotation_t& ContextIntf::qut() const {
    return _impl.qut;
}

const fund_t ContextIntf::fund() const {
    return _impl.fund;
}

Aspect* ContextIntf::load( const code_t& symbol_, const period_t& period_, int count_ ) {

    // return ASP.add( symbol_, period_, count_ );
    // todo
    return nullptr;
}

oid_t ContextIntf::open( const code_t& c_, vol_t qty_, price_t sl_, price_t tp_, price_t price_, otype_t mode_ ) {
    if ( qty_ < 0 )
        return _mgmt->sellshort( c_, -qty_, price_, mode_, "" );
    else if ( qty_ > 0 )
        return _mgmt->buylong( c_, qty_, price_, mode_, "" );
    else {
        assert( false );
        return 0;
    };

    return 0;
}

nvx_st ContextIntf::close( const code_t& c_, vol_t qty_, price_t price_, otype_t mode_ ) {
    if ( qty_ < 0 ) {
        return _mgmt->buy( c_, -qty_, price_, mode_, "" );
    }
    else if ( qty_ > 0 ) {
        return _mgmt->sell( c_, qty_, price_, mode_, "" );
    }
    else {
        return _mgmt->close( c_ );
    }
}

datetime_t ContextIntf::time() const {
    return datetime_t::now();
}

IPosition* ContextIntf::qry_long( const code_t& c_ ) {
    return _mgmt->position( c_, true );
}

IPosition* ContextIntf::qry_short( const code_t& c_ ) {

    return _mgmt->position( c_, false );
}

NVX_NS_END