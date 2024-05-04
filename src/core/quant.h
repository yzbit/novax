#ifndef C4704959_F4BE_4EAA_9C19_9CD09E83DA7D
#define C4704959_F4BE_4EAA_9C19_9CD09E83DA7D
#include <functional>

#include "ns.h"

NVX_NS_BEGIN

struct IStrategy;
struct IData;
struct ITrader;
struct Context;
struct Clock;

struct Quant {

    static Quant& instance();
    virtual ~Quant() {}

    virtual int  execute( IStrategy* s_ ) = 0;
    virtual void invoke()                 = 0;

    virtual IData*     data()     = 0;
    virtual ITrader*   trader()   = 0;
    virtual Context*   context()  = 0;
    virtual IStrategy* strategy() = 0;
};

NVX_NS_END

#define QUANT NVX_NS::Quant::instance()

#endif /* C4704959_F4BE_4EAA_9C19_9CD09E83DA7D */
