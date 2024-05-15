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

#ifndef F4D9DAEC_B9A1_48EF_8FEB_A99D60585AD3
#define F4D9DAEC_B9A1_48EF_8FEB_A99D60585AD3

#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>

#include "definitions.h"
#include "models.h"
#include "ns.h"
#include "proxy.h"
#include "ringbuffer.h"
#include "taskqueue.h"

NVX_NS_BEGIN
struct Aspect;
struct IMarket;

//
// 如果数据共享,那么start就不仅仅是调用IMarket.start了,需要统一管理不同quant的请求,然后统一处理,data实际上要作为独立的进程运行
// 那么数据服务是不是另外一个IMarket呢,Data还是本地的的data,毕竟指标的计算还是要在本地做的
// ctpagent只需要一个,数据通过zmq传递到data
// datacenter
// brokeragen
//
struct Quant;
struct Data : IData {
    Data();
    ~Data();

    void   update( const quotation_t& tick_ ) override;
    nvx_st start();
    nvx_st stop();

    Aspect* attach( const code_t& symbol_, const period_t& period_, int count_ );
    nvx_st  attach( Aspect* a_ );
    nvx_st  dettach( Aspect* a_ );

private:
    void process();

private:
    RingBuff<quotation_t, 30> _cache;
    std::list<Aspect*>        _aspects;
    TaskQueue*                _jobs = nullptr;

private:
    std::mutex              _mutex;
    std::condition_variable _cv;
};

NVX_NS_END

#endif /* F4D9DAEC_B9A1_48EF_8FEB_A99D60585AD3 */
