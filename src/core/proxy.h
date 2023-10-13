#ifndef B6D90468_E9C6_4FE4_A0C4_197D3CC5F83F
#define B6D90468_E9C6_4FE4_A0C4_197D3CC5F83F
#include "ns.h"

CUB_NS_BEGIN
struct Data;
struct Trader;

struct ProxyFactory {
    static Data::Delegator*   create_data( Data* d_, int type_ );
    static Trader::Delegator* create_trader( Trader*, t_, int type_ );
};

CUB_NS_END

#endif /* B6D90468_E9C6_4FE4_A0C4_197D3CC5F83F */
