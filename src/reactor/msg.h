#ifndef D0A82F45_7141_4EE1_ABB6_9929837DA41C
#define D0A82F45_7141_4EE1_ABB6_9929837DA41C
#include <comm/ns.h>
#include <stdint.h>

CUB_NS_BEGIN

using mid_t = uint32_t;

constexpr mid_t kExceptMsg = -1;

#pragma pack( 1 )
struct Msg {
    union {
        char  topic[ 16 ];
        mid_t id;
    };

    union {
        bool     flag;
        uint8_t  uc;
        int8_t   c;
        int      i;
        uint32_t u;
        char     data[ 64 ];
    };
};
#pragma pack()

CUB_NS_END

#endif /* D0A82F45_7141_4EE1_ABB6_9929837DA41C */
