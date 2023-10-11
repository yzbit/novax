#include "strategy.h"

CUB_NS_BEGIN

static std::unique_ptr<Strategy> _strategy_instance = nullptr;

Strategy& Strategy::instance() {
    return *_strategy_instance.get();
}

void Strategy::bind( std::unique_ptr<Strategy> s_ ) {
    _strategy_instance.swap( s_ );
}

CUB_NS_END