#include <chrono>
#include <exception>
#include <stdexcept>
#include <unistd.h>

#include "canlendar.h"
#include "context_impl.h"
#include "data.h"
#include "log.hpp"
#include "order_mgmt.h"
#include "proxy.h"
#include "quant_impl.h"
#include "strategy.h"
#include "timer.h"
#include "trader.h"

CUB_NS_BEGIN

Quant* Quant::create() {
    return new QuantImpl();
}

QuantImpl::QuantImpl() {
    init();
}

void QuantImpl::update( const quotation_t& q_ ) {
    _c->q = q_;
    _s->on_instant( q_ );
}

void QuantImpl::invoke() {
    _s->on_invoke( _c );
}

int QuantImpl::init() {
    ContextImpl* ctx = new ContextImpl( this );
    _d               = new Data( this );
    _t               = new Trader( this );
    _o               = new OrderMgmt( this );

    return 0;
}

//--处理输入,命令等
int QuantImpl::execute( Strategy* s_ ) {
    _s = s_;

    _s->on_init( _c );

    [[maybe_unused]] auto id = _timer.add(
        std::chrono::seconds( 2 ),
        [ & ]( cub::timer_id ) { this->ontick(); },
        std::chrono::seconds( 1 ) );

    while ( _running ) {
        ::sleep( 1 );
    }

    delete this;

    return 0;
}

void QuantImpl::quote( const quotation_t& q_ ) {
    _s->on_instant( q_ );

    _s->on_invoke( _c );
}

void QuantImpl::update( const order_t& o_ ) {
}

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
CUB_NS_END