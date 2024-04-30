#ifndef E93F5C75_9223_409D_8F98_DFFDE2E179BF
#define E93F5C75_9223_409D_8F98_DFFDE2E179BF

#include "models.h"
#include "ns.h"

NVX_NS_BEGIN

struct Context;
struct Quant;
struct IStrategy {
    virtual ~IStrategy() {}

    virtual void init( Quant* q_ )       = 0;
    virtual void invoke( Context* c_ )   = 0;
    virtual void prefight( Context* c_ ) = 0;
};

NVX_NS_END

#endif /* E93F5C75_9223_409D_8F98_DFFDE2E179BF */
