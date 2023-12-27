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

SATURN_NS_BEGIN
struct Aspect;
struct DataContext;

struct Data {
    struct Delegator {
        virtual ~Delegator() {}
        virtual int start()                            = 0;
        virtual int stop()                             = 0;
        virtual int subscribe( const code_t& code_ )   = 0;
        virtual int unsubscribe( const code_t& code_ ) = 0;
    };

    Data( DataContext* q_ );
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
    DataContext* _r = nullptr;

private:
    std::mutex              _mutex;
    std::condition_variable _cv;
};

SATURN_NS_END

#endif /* F4D9DAEC_B9A1_48EF_8FEB_A99D60585AD3 */
