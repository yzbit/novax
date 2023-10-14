#ifndef B87362DF_37FD_4B6A_9F1E_768AAFEA5563
#define B87362DF_37FD_4B6A_9F1E_768AAFEA5563
#include <chrono>
#include <time.h>

#include "definitions.h"
#include "ns.h"

CUB_NS_BEGIN

struct Clock {
    static Clock& of_exchange( int exid_ );

    uint32_t open_shift() { return _open_shift; }
    void     tune( const datetime_t& dt_ );  // 调整本地时间和交易所的时间
    time_t   now();

    Clock();

private:
    time_t   _epoch      = 0;
    uint32_t _drift      = 0;
    uint32_t _open_shift = 0;
};

CUB_NS_END

#define CLOCK_OF( _exid_ ) Clock::of_exchange( _exid_ )

#endif /* B87362DF_37FD_4B6A_9F1E_768AAFEA5563 */
