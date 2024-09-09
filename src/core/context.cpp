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
context_intf::context_intf( std::unique_ptr<broker> ib_, std::shared_ptr<data> d_ ) {
    _ib   = ib_.get();
    _data = d_;
}

aspect* context_intf::load( const code& symbol_, const period& period_, int count_ ) {
    auto a = _data->attach( symbol_, period_, count_ );

    if ( !a ) {
        LOG_INFO( "create aspect failed,symbol=%s, count=%d", symbol_.c_str(), count_ );
    }

    return a;
}

position* context_intf::qry_long( const code& c_ ) {
    return _mgmt.pos_of( c_, true );
}

position* context_intf::qry_short( const code& c_ ) {
    return _mgmt.pos_of( c_, false );
}

nvx_st context_intf::calloff( const oid& id_ ) {
    order* ord = _mgmt.find( id_ );

    if ( !ord
         || ( ord->status != ord_status::pending
              && ord->status != ord_status::partial_dealed
              && ord->status != ord_status::patial_canelled ) ) {
        LOG_INFO("can not cancel order, id=%u status=%d", id_, ord->status );
        return NVX_FAIL;
    }

    ord->status = ord_status::cancelling;

    return _ib->cancel( id_ );
}

oid context_intf::shorting( const code& c_, vol qty_, price price_, ord_type type_, const text& remark_ ) {
    oid id = _ib->put( c_, qty_, price_, ord_dir::p_short, 0, stop_dir::none, type_ );

    if ( id != NVX_BAD_OID ) {
        if ( NVX_OK != _mgmt.add( id, c_, qty_, price_, ord_dir::p_short, 0, stop_dir::none, type_, remark_ ) ) {
            LOG_INFO( "add to mgmt fail:%u", id );
        }
    }

    return id;
}

oid context_intf::longing( const code& c_, vol qty_, price price_, ord_type type_, const text& remark_ ) {
    oid id = _ib->put( c_, qty_, price_, ord_dir::p_long, 0, stop_dir::none, type_ );

    if ( id != NVX_BAD_OID ) {
        if ( NVX_OK != _mgmt.add( id, c_, qty_, price_, ord_dir::p_long, 0, stop_dir::none, type_, remark_ ) ) {
            LOG_INFO( "add to mgmt fail:%u", id );
        }
    }

    return id;
}

oid context_intf::trapping( const code& c_, vol qty_, price limit_, ord_dir dir_, price stop_, stop_dir sdir_, ord_type type_, const text& remark_ ) {
    oid id = _ib->put( c_, qty_, limit_, dir_, stop_, sdir_, type_ );

    if ( id != NVX_BAD_OID ) {
        if ( NVX_OK != _mgmt.add( id, c_, qty_, limit_, dir_, stop_, sdir_, type_, remark_ ) ) {
            LOG_INFO( "add to mgmt fail:%u", id );
        }
    }

    return id;
}

NVX_NS_END