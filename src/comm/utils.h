#ifndef B2AD377C_2C34_45B8_829B_C12AABBC4481
#define B2AD377C_2C34_45B8_829B_C12AABBC4481
#include <cub_ns.h>

CUB_NS_BEGIN

template <typename T>
constexpr int e2i( const T& t_ ) {
    static_cast<int>( t_ );
}

CUB_NS_END

#endif /* B2AD377C_2C34_45B8_829B_C12AABBC4481 */
