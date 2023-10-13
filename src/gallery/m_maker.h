#ifndef EBE79867_6BC9_4488_8103_9BEA34B3A708
#define EBE79867_6BC9_4488_8103_9BEA34B3A708
//从网上找到的例子,测试用

#include "../ns.h"
#include "../strategy.h"

USE_CUB_NS

struct MarketMaker : Strategy {
    void on_refresh( Context& c );
    void on_init( Context& c );
};

#endif /* EBE79867_6BC9_4488_8103_9BEA34B3A708 */
