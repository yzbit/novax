#ifndef B37DD02D_258D_4700_8440_69B0E01D2E13
#define B37DD02D_258D_4700_8440_69B0E01D2E13

#include "mdproxy.h"
#include "tradeproxy.h"

namespace ctp {
struct Source : IProvider {
    nvx_st start() override;
    nvx_st stop() override;

private:
    trader* _td;
    mdex*   _md;
};

}  // namespace  ctp

#endif /* B37DD02D_258D_4700_8440_69B0E01D2E13 */
