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

#ifndef C44A4ED8_FC43_44BB_A641_DADAFD2C62CC
#define C44A4ED8_FC43_44BB_A641_DADAFD2C62CC

#include <memory>

#include "definitions.h"
#include "models.h"
#include "ordermgmt.h"

NVX_NS_BEGIN

struct position;
struct broker;
struct data;
struct context_impl_t {
    tick   qut;
    funds  acct;
    nvxerr error;
};

struct context_intf : context {
    context_intf( std::unique_ptr<broker> ib_, std::shared_ptr<data> d_ );

    ord_mgmt& mgmt() { return _mgmt; }

private:
    aspect* load( const code& symbol_, const period& period_, int count_ ) override;

    oid shorting( const code& c_, vol qty_, price price_, ord_type type_, const text& remark_ ) override;
    oid longing( const code& c_, vol qty_, price price_, ord_type type_, const text& remark_ ) override;
    oid trapping( const code& c_, vol qty_, price limit_, ord_dir dir_, price stop_, stop_dir sdir_, ord_type type_, const text& remark_ ) override;

    nvx_st    calloff( const oid& id_ ) override;
    position* qry_long( const code& c_ ) override;
    position* qry_short( const code& c_ ) override;

private:
    context_impl_t        _impl;
    ord_mgmt              _mgmt;
    broker*               _ib   = nullptr;
    std::shared_ptr<data> _data = nullptr;
};

NVX_NS_END

#endif /* C44A4ED8_FC43_44BB_A641_DADAFD2C62CC */
