#ifndef E93F5C75_9223_409D_8F98_DFFDE2E179BF
#define E93F5C75_9223_409D_8F98_DFFDE2E179BF

#include "models.h"
#include "ns.h"

NVX_NS_BEGIN

struct Context;
struct IStrategy {
    virtual ~IStrategy() {}

    virtual void invoke( Context* context_ )    = 0;
    virtual void init( Context* context_ )      = 0;
    virtual void early( const quotation_t& q_ ) = 0;
};

NVX_NS_END

#endif /* E93F5C75_9223_409D_8F98_DFFDE2E179BF */
