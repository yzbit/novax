#ifndef AAC2F2D8_B117_447B_9C22_5AD01AD16BE1
#define AAC2F2D8_B117_447B_9C22_5AD01AD16BE1
#include "ns.h"

CUB_NS_BEGIN

/*时间用来记录每根K线开始的时间，最好还有个变量来记录是否完整*/
// char day[9];//和ctp定义的格式一样20180909
// char tm[9];//HH:MM:SS
struct datetime_t {
    int year, month, day, hour, minute, seconds, milli;
};
CUB_NS_END

#endif /* AAC2F2D8_B117_447B_9C22_5AD01AD16BE1 */
