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

#include <memory>

#include "definitions.h"
#include "models.h"
#include "ns.h"
#include "pub.h"

NVX_NS_BEGIN

struct subject {
    subject( ipub* pub_ )
        : _pub( pub_ ) {}

    virtual ~subject() {}

protected:
    int post( const pub::msg& m_ );

private:
    ipub* _pub = nullptr;
};

struct broker : subject {
    enum class type {
        ctp
    };

    broker( ipub* pub_ );

    static std::unique_ptr<broker> create( type t_, ipub* pub_ );

    virtual nvx_st start() = 0;
    virtual nvx_st stop()  = 0;

    virtual oid put( const code& sym_, vol qty_, price limit_, ord_dir odir_, price stop_, stop_dir sdir_, ord_type type_ ) = 0;

    virtual nvx_st cancel( const oid& id_ ) = 0;
};

struct market : subject {
    enum class type {
        ctp,
        dc,
        file
    };

    market( ipub* pub_ );

    static std::unique_ptr<market> create( type t_, ipub* pub_ );

    virtual nvx_st start()                          = 0;
    virtual nvx_st stop()                           = 0;
    virtual nvx_st subscribe( const code& code_ )   = 0;
    virtual nvx_st unsubscribe( const code& code_ ) = 0;
};

NVX_NS_END

#endif /* B6D90468_E9C6_4FE4_A0C4_197D3CC5F83F */
