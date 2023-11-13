#include "comm.h"

CUB_NS_BEGIN
namespace ctp {

Synchrony& Synchrony::get() {
    static Synchrony _instance;

    return _instance;
}
}  // namespace ctp

CUB_NS_END