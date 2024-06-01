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

#include "definitions.h"
#include "models.h"

NVX_NS_BEGIN
struct aspect;
struct position;

struct context {
    static context* create();

    virtual const tick& qut() const        = 0;
    virtual const funds acct() const       = 0;
    virtual aspect*     load( const code&   symbol_,
                              const period& period_,
                              int           count_ ) = 0;

    virtual oid open( const code& c_,
                      vol         qty_,
                      price       sl_    = 0,
                      price       tp_    = 0,
                      price       price_ = 0,
                      ord_type    mode_  = ord_type::market ) = 0;

    virtual nvx_st    close( const code& c_,
                             vol         qty_,
                             price       price_ = 0,
                             ord_type    mode_  = ord_type::market ) = 0;
    virtual position* qry_long( const code& c_ )                 = 0;
    virtual position* qry_short( const code& c_ )                = 0;
    virtual datetime  time() const                               = 0;
    virtual nvxerr_t  error() const                              = 0;
    virtual ~context();
};

NVX_NS_END

#endif /* C2E26F98_58D2_4FB6_9B05_CB4ED59A65C3 */
