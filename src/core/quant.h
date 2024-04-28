#ifndef C4704959_F4BE_4EAA_9C19_9CD09E83DA7D
#define C4704959_F4BE_4EAA_9C19_9CD09E83DA7D
#include "ns.h"

NVX_NS_BEGIN

struct Strategy;

//-是否有必要运行多个实例
//-不同的策略是可以共享一个数据源,一个订单管理模块的
//-
struct Quant {
    static Quant* create();

    virtual ~Quant() {}
    virtual int execute( Strategy* s_ ) = 0;

protected:
    Quant() {}
};

NVX_NS_END

#endif /* C4704959_F4BE_4EAA_9C19_9CD09E83DA7D */
