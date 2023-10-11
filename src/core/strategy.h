#ifndef E93F5C75_9223_409D_8F98_DFFDE2E179BF
#define E93F5C75_9223_409D_8F98_DFFDE2E179BF
#include <any>
#include <array>
#include <functional>
#include <memory>
#include <optional>
#include <string>

#include "context.h"
#include "definitions.h"
#include "models.h"
#include "ns.h"

CUB_NS_BEGIN

struct Strategy {
    static Strategy& instance();
    static void      bind( std::unique_ptr<Strategy> s_ );

    virtual ~Strategy() {}

    virtual void on_invoke( Context* context_ )      = 0;
    virtual void on_init( Context* context_ )        = 0;
    virtual void on_instant( const quotation_t& q_ ) = 0;
};

CUB_NS_END

#define STRATEGY cub::Strategy::instance()
#define BIND_STRATEGY( _s_ ) cub::Strategy::bind( std::move( _s_ ) )

#endif /* E93F5C75_9223_409D_8F98_DFFDE2E179BF */
