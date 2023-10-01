#ifndef FF72955C_ACC6_4839_BA9D_5BA5BB151C3F
#define FF72955C_ACC6_4839_BA9D_5BA5BB151C3F
#include "ns.h"

CUB_NS_BEGIN

// 用来管理交易策略，而真正的交易策略叫algo
struct Strategy {
    static Strategy& instance();

    void fire( const void* ) {}
    void invoke( const void* ) {}
};

CUB_NS_END

#define STRATEGY cub::Strategy::instance()

#endif /* FF72955C_ACC6_4839_BA9D_5BA5BB151C3F */
