#ifndef F4D9DAEC_B9A1_48EF_8FEB_A99D60585AD3
#define F4D9DAEC_B9A1_48EF_8FEB_A99D60585AD3

#include <list>
#include <memory>

#include "definitions.h"
#include "models.h"
#include "msg.h"
#include "ns.h"
#include "ring_buffer.h"
#include "task_queue.h"

CUB_NS_BEGIN
struct Quant;

struct Data {
    Data( Quant* q_ );
    virtual ~Data();

    virtual int start()                            = 0;
    virtual int stop()                             = 0;
    virtual int subscribe( const code_t& code_ )   = 0;
    virtual int unsubscribe( const code_t& code_ ) = 0;

private:
    void update( const quotation_t& tick_ );

private:
    void process();

private:
    TaskQueue*                _jobs = nullptr;
    RingBuff<quotation_t, 30> _cache;
    std::list<Aspect*>        _aspects;
    Quant*                    _q;
};

CUB_NS_END

#define DATA cub::Data::instance()

#endif /* F4D9DAEC_B9A1_48EF_8FEB_A99D60585AD3 */
