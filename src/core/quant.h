#ifndef C4704959_F4BE_4EAA_9C19_9CD09E83DA7D
#define C4704959_F4BE_4EAA_9C19_9CD09E83DA7D
#include "definitions.h"
#include "models.h"
#include "ns.h"
#include "strategy.h"
#include "timer.h"

CUB_NS_BEGIN

// 如果允许运行多个quant实例,其他的就不能是单体类
// 时钟可以是全局的
struct Data;
struct Context;
struct Trader;
struct Strategy;

struct Quant {
    Quant();

    virtual ~Quant() {}
    int init();
    int execute( Strategy* s_ );

    //--不想让最终用户知道这些信息，只想在data，trader中共享彼此
    Data*    d();
    Trader*  t();
    Strategy s();

private:
    void quote( const quotation_t& q_ );
    void update( const order_t& o_ );

private:
    void ontick();

private:
    Data*     _d;
    Trader*   _t;
    Strategy* _s;
    Context*  _c;

private:
    bool  _working;
    Timer _timer;
};

CUB_NS_END

#endif /* C4704959_F4BE_4EAA_9C19_9CD09E83DA7D */
