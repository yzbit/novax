#ifndef E93F5C75_9223_409D_8F98_DFFDE2E179BF
#define E93F5C75_9223_409D_8F98_DFFDE2E179BF
#include <any>
#include <array>
#include "ns.h"
#include <functional>
#include <optional>
#include <string>

CUB_NS_BEGIN

struct AlgoContext {
};

struct Algo {
    virtual ~Algo() {}
    virtual void on_refresh( AlgoContext* context_ ) = 0;
};

struct AlgoArg {
    std::any value;

    operator int() const {
        return std::any_cast<int>( value );
    }

    operator const char*() const {
        return std::any_cast<const char*>( value );
    }

    operator std::string() const {
        return std::any_cast<std::string>( value );
    }
};

static constexpr int kMaxArgsSupport = 16;

using ArgPack = std::array<AlgoArg, kMaxArgsSupport>;

CUB_NS_END
#endif /* E93F5C75_9223_409D_8F98_DFFDE2E179BF */
