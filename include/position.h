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

#ifndef F3D31FE0_A591_4B61_A625_E6EC52FC5817
#define F3D31FE0_A591_4B61_A625_E6EC52FC5817
#include "definitions.h"

NVX_NS_BEGIN

struct position {
    enum class type_t {
        relative,
        absolute
    };

    /**stop和gain是修改止损止盈单,参数全0会删除止损止盈单,只允许下一个止损或者止盈
     * 可以通过条件单自己设置更多止损
     */
    virtual nvx_st stop( vol qty_, price price_, type_t t_ ) = 0;
    virtual nvx_st gain( vol qty_, price price_ )            = 0;
    virtual price  avg_dealt()                               = 0;
    virtual vol    qty()                                     = 0;
};

NVX_NS_END

#endif /* F3D31FE0_A591_4B61_A625_E6EC52FC5817 */
