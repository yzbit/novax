#ifndef C4704959_F4BE_4EAA_9C19_9CD09E83DA7D
#define C4704959_F4BE_4EAA_9C19_9CD09E83DA7D
#include "definitions.h"
#include "models.h"
#include "ns.h"
#include "strategy.h"
#include "timer.h"

CUB_NS_BEGIN

//todo
// 如果允许运行多个quant实例,其他的就不能是单体类
// 时钟可以是全局的
struct Quant {
    static Quant* create();

    virtual ~Quant() {}
    virtual int init()                  = 0;
    virtual int execute( Strategy* s_ ) = 0;

private:
    Quant();
};

CUB_NS_END

#endif /* C4704959_F4BE_4EAA_9C19_9CD09E83DA7D */
