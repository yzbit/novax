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

context_intf::context_intf( order_mgmt* m_ )
    : _mgmt( m_ ) {}

void context_intf::update_qut( const tick& qut_ ) {
    _impl.qut = qut_;
}

void context_intf::update_fund( const funds& fund_ ) {
    _impl.acct = fund_;
}

void context_intf::update_error( const nvxerr_t& err_ ) {
    _impl.error = err_;
}

nvxerr_t context_intf::error() const {
    return _impl.error;
}

const tick& context_intf::qut() const {
    return _impl.qut;
}

const funds context_intf::acct() const {
    return _impl.acct;
}

aspect* context_intf::load( const code& symbol_, const period& period_, int count_ ) {

    // return ASP.add( symbol_, period_, count_ );
    // todo
    return nullptr;
}

oid context_intf::open( const code& c_, vol qty_, price sl_, price tp_, price price_, ord_type mode_ ) {
    if ( qty_ < 0 )
        return _mgmt->sellshort( c_, qty_, price_, mode_, "" );
    else if ( qty_ > 0 )
        return _mgmt->buylong( c_, qty_, price_, mode_, "" );
    else {
        assert( false );
        return 0;
    };

    return 0;
}

oid context_intf::close( const code& c_, vol qty_, price price_, ord_type mode_ ) {
    if ( qty_ < 0 ) {
        return _mgmt->buy( c_, qty_, price_, mode_, "" );
    }
    else if ( qty_ > 0 ) {
        return _mgmt->sell( c_, qty_, price_, mode_, "" );
    }
    else {
        return _mgmt->close( c_ );
    }
}

datetime context_intf::time() const {
    return datetime::now();
}

position* context_intf::qry_long( const code& c_ ) {
    return _mgmt->pos_of( c_, true );
}

position* context_intf::qry_short( const code& c_ ) {
    return _mgmt->pos_of( c_, false );
}

NVX_NS_END