#ifndef C4704959_F4BE_4EAA_9C19_9CD09E83DA7D
#define C4704959_F4BE_4EAA_9C19_9CD09E83DA7D
#include "ns.h"

NVX_NS_BEGIN

struct IStrategy;
struct IData;
struct ITrader;
struct Context;
struct Clock;

struct Quant final {
    int        execute( IStrategy* s_ );
    void       invoke();
    IData*     data();
    ITrader*   trader();
    Context*   context();
    Clock*     clock();
    IStrategy* strategy();

    ~Quant();
    Quant();

private:
    IData*     _d     = nullptr;
    ITrader*   _t     = nullptr;
    IStrategy* _s     = nullptr;
    Context*   _c     = nullptr;
    Clock*     _clock = nullptr;
};

NVX_NS_END

#endif /* C4704959_F4BE_4EAA_9C19_9CD09E83DA7D */
