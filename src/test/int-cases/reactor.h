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

#ifndef CD744141_B668_45C8_AED6_852558124F13
#define CD744141_B668_45C8_AED6_852558124F13

#include <functional>
#include <set>

#include "msg.h"
#include "ns.h"

NVX_NS_BEGIN

using mid_set_t     = std::set<msg::mid_t>;
using msg_handler_t = std::function<void( const msg::header_t& h_ )>;

struct Reactor {
    virtual ~Reactor() {}

    static Reactor& instance();

    template <typename T>
    int pub( const T& m_ ) {
        return pub( &m_, sizeof( T ) );
    }

    virtual int pub( const void* data_, size_t length_ )           = 0;
    virtual int sub( const mid_set_t& msg_set_, msg_handler_t h_ ) = 0;
};

NVX_NS_END

#define REACTOR NVX_NS::Reactor::instance()

#endif /* CD744141_B668_45C8_AED6_852558124F13 */
