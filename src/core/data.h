#ifndef F4D9DAEC_B9A1_48EF_8FEB_A99D60585AD3
#define F4D9DAEC_B9A1_48EF_8FEB_A99D60585AD3

#include <list>
#include <memory>

#include "definitions.h"
#include "models.h"
#include "msg.h"
#include "ns.h"
#include "task_queue.h"

CUB_NS_BEGIN
class Indicator;
class Market;
class Aspect;

struct Data {
    static Data& instance();

    Data();
    virtual ~Data();

    virtual int start() { return 0; }
    virtual int stop() { return 0; }

    virtual int subscribe( const code_t& code_ );
    virtual int unsubscribe( const code_t& code_ );

private:
    void on_data( const quotation_t& tick_ );
    //  void on_msg( const msg::header_t& h );

private:
    std::list<Aspect*> _aspects;
    TaskQueue*         _jobs      = nullptr;
    Indicator*         _indicator = nullptr;
};

CUB_NS_END
#define DATA cub::Data::instance()

#endif /* F4D9DAEC_B9A1_48EF_8FEB_A99D60585AD3 */
