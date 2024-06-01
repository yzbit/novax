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
#include "pub.h"
NVX_NS_BEGIN

struct subject {
    subject( pub* pub_ )
        : _pub( pub_ ) {}

    virtual ~subject() {}

protected:
    int post( const pub::msg_t& m_ );

private:
    pub* _pub = nullptr;
};

struct broker : subject {
    enum class Type {
        ctp
    };

    broker( pub* pub_ );
    static broker* create( Type t_, pub* pub_ );

    virtual nvx_st start()             = 0;
    virtual nvx_st stop()              = 0;
    virtual oid    put( const code& instrument_,
                        vol         qty_,
                        price       price_,
                        otype       mode_,
                        ord_dir     dir_ ) = 0;

    virtual nvx_st cancel( const oid& id_ ) = 0;
};

struct market : subject {
    enum class
        static market*
        create( Type t_, pub* pub_ );

    market( pub* pub_ );
    virtual nvx_st start()                          = 0;
    virtual nvx_st stop()                           = 0;
    virtual nvx_st subscribe( const code& code_ )   = 0;
    virtual nvx_st unsubscribe( const code& code_ ) = 0;
};

NVX_NS_END

#endif /* B6D90468_E9C6_4FE4_A0C4_197D3CC5F83F */
