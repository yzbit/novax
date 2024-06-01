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

* \author: yaozn(zinan@outlook.com), qianq(695997058@qq.com)
* \date: 2024
**********************************************************************************/

#ifndef B37DD02D_258D_4700_8440_69B0E01D2E13
#define B37DD02D_258D_4700_8440_69B0E01D2E13

#include "mdproxy.h"
#include "tradeproxy.h"

// trader先调用ReqQryInstrument查询所有合约,然后再调用md的sub函数
namespace ctp {
struct source : NVX_NS::provider {
    nvx_st start() override;
    nvx_st stop() override;

private:
    trader* _td;
    mdex*   _md;
};

}  // namespace  ctp

#endif /* B37DD02D_258D_4700_8440_69B0E01D2E13 */
