#ifndef B87362DF_37FD_4B6A_9F1E_768AAFEA5563
#define B87362DF_37FD_4B6A_9F1E_768AAFEA5563
#include <comm/datetime.h>
#include <cub_ns.h>

CUB_NS_BEGIN

struct Clock {
    void tune( const Datetime& dt_ );
};

CUB_NS_END

#endif /* B87362DF_37FD_4B6A_9F1E_768AAFEA5563 */
