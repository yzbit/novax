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

#ifndef BB30CC9E_0827_4895_93A9_D7B8DC7EA5FB
#define BB30CC9E_0827_4895_93A9_D7B8DC7EA5FB
#include <ns.h>
#include <strategy.h>

USE_CUB_NS

struct BreakTh : Strategy {
    BreakTh( const code_t& code_ )
        : _code( code_ ) {}

    void on_invoke( Context* c ) override;
    void on_init( Context* c ) override;
    void on_ck( const quotation_t& ) override;

private:
    code_t     _code;
    Indicator* _ma = nullptr;
};

#endif /* BB30CC9E_0827_4895_93A9_D7B8DC7EA5FB */
