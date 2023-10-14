#ifndef FA53C2F6_3D31_46A5_A112_0E4AA8FF9E25
#define FA53C2F6_3D31_46A5_A112_0E4AA8FF9E25
#include "models.h"
#include "ns.h"

CUB_NS_BEGIN

// context as role
struct DataContext {
    virtual void update( const quotation_t& q_ ) = 0;
    virtual void invoke()                        = 0;
};

struct MgmtContext {
    virtual int put_order( const order_t& o_ ) = 0;
    virtual int del_order( oid_t id_ )         = 0;
};

CUB_NS_END

#endif /* FA53C2F6_3D31_46A5_A112_0E4AA8FF9E25 */
