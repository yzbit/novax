#ifndef B791BD3D_D69F_45B3_8E6C_D92191157DA5
#define B791BD3D_D69F_45B3_8E6C_D92191157DA5
// 市场相关的所有特新,包括日历和每天的交易时间等
#include <cub_ns.h>
#include <memory>

CUB_NS_BEGIN

struct Market {
    Market();
    virtual ~Market();
    static Market* create();

    virtual int init() { return 0; }
    virtual int subscribue( const code_t& code_ ) { return 0; }
    virtual int unsubscribue( const code_t& code_ ) { return 0; }
};

CUB_NS_END

#endif /* B791BD3D_D69F_45B3_8E6C_D92191157DA5 */
