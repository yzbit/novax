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

#ifndef B4872862_3AFF_45FF_86DA_A0808D740978
#define B4872862_3AFF_45FF_86DA_A0808D740978
#include "definitions.h"
#include "models.h"

NVX_NS_BEGIN

struct IIndicator {
    enum class IndiType {
        main,
        addi
    };

    virtual const char* name() const { return "indi##"; }
    virtual size_t      nvalue() { return 1; }
    virtual IndiType    type() { return IndiType::main; }
    virtual double      at( int track_, int index_ ) { return .0; }

    virtual ~IIndicator() {}
};

struct IAlgo {
    virtual void calc( const quotation_t& qut_, int total_bars_ ) = 0;

    virtual ~IAlgo() {}
};

// struct Ma:IAlogo, IIndicator
NVX_NS_END

#endif /* B4872862_3AFF_45FF_86DA_A0808D740978 */
