#ifndef B1DEFD41_1804_4B09_B322_42B7E58CD280
#define B1DEFD41_1804_4B09_B322_42B7E58CD280

#include "../definitions.h"
#include "../indicator.h"
#include "../ns.h"

CUB_NS_BEGIN

struct Ma : Indicator {
    static Ma* create( const arg_pack_t& p_ );

    Ma( const std::string& code_, int period_ );
};

CUB_NS_END
#endif /* B1DEFD41_1804_4B09_B322_42B7E58CD280 */
