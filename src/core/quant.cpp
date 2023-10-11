#include <chrono>
#include <exception>
#include <stdexcept>
#include <unistd.h>

#include "application.h"
#include "canlendar.h"
#include "data.h"
#include "log.hpp"
#include "strategy.h"
#include "timer.h"
#include "trader.h"

CUB_NS_BEGIN

static void wall_clock() {
    static bool _trading = false;

    if ( !CANLEN.is_trading_day() ) {
        _trading = false;
        return;
    }

    auto d = CANLEN.trading_time();
    auto t = datetime_t::now();

    if ( !_trading ) {
        auto diff = d.start.hour * 3600 + d.start.minute * 60
                    - t.hour * 3600 - 60 * t.minute;

        if ( diff >= 0 && diff < 5 * 60 ) {
            LOG_INFO( "market open" );
            DATA.start();
            TRADER.start();

            _trading = true;
        }
    }

    if ( _trading ) {
        auto diff = t.hour * 3600 - 60 * t.minute
                    - d.end.hour * 3600 + d.end.minute * 60;

        if ( diff > 5 * 60 ) {
            DATA.stop();
            TRADER.stop();
            _trading = true;
        }
    }
}

int Quant::init() {
    static Timer _t;

    [[maybe_unused]] auto id = _t.add(
        std::chrono::seconds( 2 ),
        []( cub::timer_id ) { wall_clock(); },
        std::chrono::seconds( 1 ) );

    //创建context,然后把所有的东西都和context进行绑定
    STRATEGY.on_init();

    return 0;
}

//--处理输入,命令等
int Quant::exec() {
    for ( ;; ) {
        ::sleep( 1 );
    }

    return 0;
}

CUB_NS_END