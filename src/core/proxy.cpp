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

#include "proxy.h"

#include "ctp/mdproxy.h"
#include "ctp/tradeproxy.h"
#include "datacenter.h"

NVX_NS_BEGIN

int subject::post( const pub::msg& m_ ) {
    return _pub ? _pub->post( m_ ) : -1;
}

broker::broker( ipub* pub_ )
    : subject( pub_ ){};

market::market( ipub* pub_ )
    : subject( pub_ ) {
}

market* create_market( ipub* p_ ) {
    return new dc_client( p_ );
}

broker* create_broker( ipub* p_ ) {
    return new ctp::trader( p_ );
}

NVX_NS_END