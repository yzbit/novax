#ifndef F4D9DAEC_B9A1_48EF_8FEB_A99D60585AD3
#define F4D9DAEC_B9A1_48EF_8FEB_A99D60585AD3

#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>

#include "definitions.h"
#include "models.h"
#include "msg.h"
#include "ns.h"
#include "proxy.h"
#include "ring_buffer.h"
#include "task_queue.h"

NVX_NS_BEGIN
struct Aspect;
struct IMarket;

//
//如果数据共享,那么start就不仅仅是调用IMarket.start了,需要统一管理不同quant的请求,然后统一处理,data实际上要作为独立的进程运行
//那么数据服务是不是另外一个IMarket呢,Data还是本地的的data,毕竟指标的计算还是要在本地做的
//ctpagent只需要一个,数据通过zmq传递到data
//datacenter
//brokeragen
//
struct Data : IData {
    Data();
    ~Data();

    static Data& instance();

    void update( const quotation_t& tick_ ) override;

    //--这些命令必须通过消息下发给ctp agent
    int  start();
    int  stop();
    int  subscribe( const code_t& code_ );
    int  unsubscribe( const code_t& code_ );

    Aspect* attach( const code_t& symbol_, const period_t& period_, int count_ );
    int     attach( Aspect* a_ );

private:
    void process();

private:
    RingBuff<quotation_t, 30> _cache;
    std::list<Aspect*>        _aspects;
    TaskQueue*                _jobs = nullptr;

private:
    std::mutex              _mutex;
    std::condition_variable _cv;
    IMarket*                _m = nullptr;
};

NVX_NS_END

#define DATA Data::instance()

#endif /* F4D9DAEC_B9A1_48EF_8FEB_A99D60585AD3 */
