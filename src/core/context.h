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

#ifndef C2E26F98_58D2_4FB6_9B05_CB4ED59A65C3
#define C2E26F98_58D2_4FB6_9B05_CB4ED59A65C3
#include <atomic>
#include <memory>

#include "clock.h"
#include "definitions.h"
#include "kline.h"
#include "models.h"
#include "ns.h"

NVX_NS_BEGIN
struct Aspect;
struct Quant;
struct Context {
    const quotation_t& qut() const;
    const fund_t       fund() const;

    Aspect* load( const code_t& symbol_, const period_t& period_, int count_ );
    oid_t   open( const code_t& c_, vol_t qty_, price_t sl_ = 0, price_t tp_ = 0, price_t price_ = 0, otype_t mode_ = otype_t::market );
    nvx_st  close( const code_t& c_, vol_t qty_, price_t price_ = 0, otype_t mode_ = otype_t::market );
    vol_t   position() const;
    vol_t   position( const code_t& c_ ) const;
    vol_t   pending() const;
    vol_t   pending( const code_t& c_ ) const;
    price_t put_price() const;
    price_t last_deal() const;
    kidx_t  last_entry() const;
    kidx_t  last_exit() const;

    Context();
    void update_qut( const quotation_t& q_ );
    void update_fund( const fund_t& f_ );

private:
    quotation_t         _qut;
    std::atomic<fund_t> _fund;
};

NVX_NS_END

#endif /* C2E26F98_58D2_4FB6_9B05_CB4ED59A65C3 */
