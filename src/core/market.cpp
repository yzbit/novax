#include "market.h"

#include "ctp_proxy.h"

CUB_NS_BEGIN
Market::Market() {
}

Market::~Market() {
}

Market* Market::create() {
    return new CtpExMd();
}

int Market::init() {
    return 0;
}

CUB_NS_END