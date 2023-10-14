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
#include "ring_buffer.h"
#include "task_queue.h"

CUB_NS_BEGIN
struct Aspect;
struct QuantImpl;

struct Data {
    struct Delegator {
        virtual ~Delegator() {}
        virtual int start()                            = 0;
        virtual int stop()                             = 0;
        virtual int subscribe( const code_t& code_ )   = 0;
        virtual int unsubscribe( const code_t& code_ ) = 0;
    };

    Data( QuantImpl* q_ );
    ~Data();

    void update( const quotation_t& tick_ );

    int     start();
    int     stop();
    int     subscribe( const code_t& code_ );
    int     unsubscribe( const code_t& code_ );
    Aspect* attach( const code_t& symbol_, const period_t& period_, int count_ );
    int     attach( Aspect* a_ );

private:
    void process();

private:
    RingBuff<quotation_t, 30> _cache;
    std::list<Aspect*>        _aspects;
    Delegator*                _d    = nullptr;
    TaskQueue*                _jobs = nullptr;

private:
    QuantImpl* _q = nullptr;

private:
    std::mutex              _mutex;
    std::condition_variable _cv;
};

CUB_NS_END

#define DATA cub::Data::instance()

#endif /* F4D9DAEC_B9A1_48EF_8FEB_A99D60585AD3 */
