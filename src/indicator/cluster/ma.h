#ifndef B1DEFD41_1804_4B09_B322_42B7E58CD280
#define B1DEFD41_1804_4B09_B322_42B7E58CD280
#include <cub.h>

#include "algo.h"

CUB_NS_BEGIN

struct Ma : Algo {
    static Ma* create( const ArgPack& p_ );
    Ma( const std::string& code_, int period_ );
};

CUB_NS_END
#endif /* B1DEFD41_1804_4B09_B322_42B7E58CD280 */
