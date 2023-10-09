#include <chrono>
#include <unistd.h>

#include "application.h"

#include "data.h"
#include "log.hpp"
#include "timer.h"
#include "trader.h"

CUB_NS_BEGIN

static void wall_clock() {
    if ( market_open() ) {
        DATA.start();
        TRADER.start();
    }

    if ( market_close() ) {
        DATA.stop();
        TRADER.stop();
    }
}

int App::init() {
    static Timer _t;

    auto id = _t.add(
        std::chrono::seconds( 2 ),
        []( cub::timer_id ) { wall_clock(); },
        std::chrono::seconds( 1 ) );

    STRATEGY.init();

    return 0;
}

//--处理输入,命令等
int App::exec() {
    for ( ;; ) {
        ::sleep( 1 );
    }

    return 0;
}

CUB_NS_END