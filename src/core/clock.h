#ifndef B87362DF_37FD_4B6A_9F1E_768AAFEA5563
#define B87362DF_37FD_4B6A_9F1E_768AAFEA5563
#include <chrono>

#include "definitions.h"
#include "ns.h"

CUB_NS_BEGIN

struct Clock {
    static Clock& of_exchange( int exid_ );

    void tune( const datetime_t& dt_ );  // 调整本地时间和交易所的时间
    int  now();

private:
    std::chrono::time_point _;
};

CUB_NS_END

#define CLOCK_OF( _exid_ ) Clock::of_exchange( _exid_ )

#endif /* B87362DF_37FD_4B6A_9F1E_768AAFEA5563 */
