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

struct IPosition;
struct OrderMgmt;

struct context_impl_t {
    quotation_t qut;
    fund_t      fund;
    nvxerr_t    error;
};

struct ContextIntf : IContext {
    ContextIntf( OrderMgmt* mgmt_ );

    void update_qut( const quotation_t& qut_ );
    void update_fund( const fund_t& fund_ );
    void update_error( const nvxerr_t& err_ );

private:
    const quotation_t& qut() const override;
    const fund_t       fund() const override;
    Aspect*            load( const code_t&   symbol_,
                             const period_t& period_,
                             int             count_ ) override;

    oid_t open( const code_t& c_,
                vol_t         qty_,
                price_t       sl_    = 0,
                price_t       tp_    = 0,
                price_t       price_ = 0,
                otype_t       mode_  = otype_t::market );

    nvx_st close( const code_t& c_,
                  vol_t         qty_,
                  price_t       price_ = 0,
                  otype_t       mode_  = otype_t::market );

    IPosition* qry_long( const code_t& c_ ) override;
    IPosition* qry_short( const code_t& c_ ) override;
    datetime_t time() const override;
    nvxerr_t   error() const override;

private:
    context_impl_t _impl;
    OrderMgmt*     _mgmt;
};

NVX_NS_END

#endif /* C44A4ED8_FC43_44BB_A641_DADAFD2C62CC */
