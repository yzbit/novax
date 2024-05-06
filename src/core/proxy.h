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

#ifndef B6D90468_E9C6_4FE4_A0C4_197D3CC5F83F
#define B6D90468_E9C6_4FE4_A0C4_197D3CC5F83F

#include "definitions.h"
#include "models.h"
#include "ns.h"

NVX_NS_BEGIN

struct IBroker;
struct ITrader {
    virtual void update_ord( oid_t id_, ostatus_t status_ ) = 0;
    virtual void update_ord( const order_t& o_ )            = 0;
    virtual void update_fund( const fund_t& f_ )            = 0;
    virtual void update_position()                          = 0;
    virtual ~ITrader();

protected:
    IBroker* ib() { return _ib; }

private:
    IBroker* _ib;
    friend IBroker;
};

struct IBroker {
    IBroker( ITrader* tr_ );
    virtual ~IBroker();

    virtual int start()                     = 0;
    virtual int stop()                      = 0;
    virtual int put( const order_t& o_ )    = 0;
    virtual int cancel( const order_t& o_ ) = 0;

protected:
    ITrader* delegator();

private:
    ITrader* _tr = nullptr;
};

struct IMarket;
struct IData {
    virtual void update( const quotation_t& tick_ ) = 0;
    virtual ~IData();

protected:
    IMarket* market() { return _m; }

private:
    IMarket* _m;
    friend IMarket;
};

struct IMarket {
    IMarket( IData* dt_ );

    virtual int start()                            = 0;
    virtual int stop()                             = 0;
    virtual int subscribe( const code_t& code_ )   = 0;
    virtual int unsubscribe( const code_t& code_ ) = 0;

    virtual ~IMarket();

protected:
    IData* delegator();

private:
    IData* _dt = nullptr;
};

NVX_NS_END

#endif /* B6D90468_E9C6_4FE4_A0C4_197D3CC5F83F */
