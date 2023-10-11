#ifndef C4704959_F4BE_4EAA_9C19_9CD09E83DA7D
#define C4704959_F4BE_4EAA_9C19_9CD09E83DA7D
#include "data.h"
#include "ns.h"
#include "strategy.h"
#include "trader.h"

CUB_NS_BEGIN

//如果允许运行多个quant实例,其他的就不能是单体类
//时钟可以是全局的
struct Quant {
    Quant();

    virtual ~Quant() {}
    virtual int init();
    virtual int exec();

#if 0
    OrderMgmt* omgmt();
    Trader*    trader();
    Data*      data();
    Strategy*  strategy();
#endif
};

CUB_NS_END

#endif /* C4704959_F4BE_4EAA_9C19_9CD09E83DA7D */
