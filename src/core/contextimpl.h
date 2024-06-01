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

#include "definitions.h"
#include "models.h"

NVX_NS_BEGIN

struct position;
struct order_mgmt;

struct context_impl_t {
    tick     qut;
    funds    acct;
    nvxerr_t error;
};

struct context_intf : context {
    context_intf( order_mgmt* mgmt_ );

    void update_qut( const tick& qut_ );
    void update_fund( const funds& fund_ );
    void update_error( const nvxerr_t& err_ );

private:
    const tick& qut() const override;
    const funds acct() const override;
    aspect*     load( const code&   symbol_,
                      const period& period_,
                      int           count_ ) override;

    oid open( const code& c_,
              vol         qty_,
              price       sl_    = 0,
              price       tp_    = 0,
              price       price_ = 0,
              ord_type    mode_  = ord_type::market );

    nvx_st close( const code& c_,
                  vol         qty_,
                  price       price_ = 0,
                  ord_type    mode_  = ord_type::market );

    position* qry_long( const code& c_ ) override;
    position* qry_short( const code& c_ ) override;
    datetime  time() const override;
    nvxerr_t  error() const override;

private:
    context_impl_t _impl;
    order_mgmt*    _mgmt;
};

NVX_NS_END

#endif /* C44A4ED8_FC43_44BB_A641_DADAFD2C62CC */
