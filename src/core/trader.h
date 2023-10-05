#ifndef AC98F1C0_3A88_4649_B85A_1852F9A06C67
#define AC98F1C0_3A88_4649_B85A_1852F9A06C67

#include "ns.h"
CUB_NS_BEGIN

struct Trader {
    static Trader* create();

    virtual ~Trader() {}
    virtual int put( const order_t& o_ ) { return 0; }
    virtual int cancel( oid_t o_ ) {
        return 0;
    };
}

CUB_NS_END

#endif /* AC98F1C0_3A88_4649_B85A_1852F9A06C67 */
