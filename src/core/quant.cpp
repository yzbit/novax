#include <unistd.h>

#include "quant.h"

#include "clock.h"
#include "context.h"
#include "data.h"
#include "order_mgmt.h"
#include "strategy.h"

NVX_NS_BEGIN

struct QuantImpl : Quant {

    QuantImpl() {
        _c     = new Context();
        _d     = new Data();
        _t     = new OrderMgmt();
        _clock = new Clock();
    }

    ~QuantImpl() {
        delete _c;
        delete _t;
        delete _d;
        delete _clock;
    }

    int        execute( IStrategy* s_ ) override;
    void       invoke() override { _s->invoke( context() ); }
    IData*     data() override { return _d; }
    ITrader*   trader() override { return _t; }
    Context*   context() override { return _c; }
    Clock*     clock() override { return _clock; }
    IStrategy* strategy() override { return _s; }

    IData*     _d     = nullptr;
    ITrader*   _t     = nullptr;
    IStrategy* _s     = nullptr;
    Context*   _c     = nullptr;
    Clock*     _clock = nullptr;
};

Quant& Quant::instance() {
    static QuantImpl _impl;
    return _impl;
}

//--处理输入,命令等
int QuantImpl::execute( IStrategy* s_ ) {
    _s = s_;

    _s->init();

#if 0
    [[maybe_unused]] auto id = _timer.add(
        std::chrono::seconds( 2 ),
        [ & ]( cub::timer_id ) { this->ontick(); },
        std::chrono::seconds( 1 ) );
#endif

    for ( ;; )
        ::sleep( 1 );

    delete this;
    return 0;
}

/*
void QuantImpl::ontick() {
    if ( !CANLEN.is_trading_day() ) {
        _working = false;
        return;
    }

    auto d = CANLEN.trading_time();
    auto t = datetime_t::now();

    if ( !_working ) {
        auto diff = d.start.hour * 3600 + d.start.minute * 60
                    - t.hour * 3600 - 60 * t.minute;

        if ( diff >= 0 && diff < 5 * 60 ) {
            LOG_INFO( "market open" );
            _d->start();
            _t->start();

            _working = true;
        }
    }

    if ( _working ) {
        auto diff = t.hour * 3600 - 60 * t.minute
                    - d.end.hour * 3600 + d.end.minute * 60;

        if ( diff > 5 * 60 ) {
            _d->stop();
            _t->stop();

            _working = true;
        }
    }
}
*/
NVX_NS_END