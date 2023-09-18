#include "reactor.h"

CUB_NS_BEGIN

Reactor& Reactor::instance() {
    static Reactor r;
    return r;
}

CUB_NS_END