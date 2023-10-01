#include "strategy.h"

CUB_NS_BEGIN

Strategy& Strategy::instance() {
    static Strategy s;
    return s;
}

CUB_NS_END
