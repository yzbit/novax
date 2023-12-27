#ifndef B6D90468_E9C6_4FE4_A0C4_197D3CC5F83F
#define B6D90468_E9C6_4FE4_A0C4_197D3CC5F83F
#include "data.h"
#include "ns.h"
#include "order_mgmt.h"

SATURN_NS_BEGIN

struct ProxyFactory {
    static Data::Delegator*      create_data( Data* d_, int type_ );
    static OrderMgmt::Delegator* create_trader( OrderMgmt* t_, int type_ );
};

SATURN_NS_END

#endif /* B6D90468_E9C6_4FE4_A0C4_197D3CC5F83F */
