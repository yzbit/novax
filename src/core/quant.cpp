#include <chrono>
#include <exception>
#include <stdexcept>
#include <unistd.h>

#include "quant.h"

#include "canlendar.h"
#include "data.h"
#include "log.hpp"
#include "proxy.h"
#include "strategy.h"
#include "timer.h"
#include "trader.h"

CUB_NS_BEGIN

static void wall_clock() {
}

// dci
// domain context interaction
//
// 不同的用户场景需要的context/role是不一样的,数据可以是推，也可以是拉
//--context主要是给strategy用的，
// 计算的时候也需要的，但是需要的context内容1不一样，也就是role不一样--从dci模式的角度去思考
// algo 需要知道aspect，抑或其他的aspect，并不需要context，一个algo只可能基于一个aspect创建，它没必要访问其他的aspect，想看到多个aspect那就再strategy中新增一个函数好了
// strategy需要知道一些下单接口，需要知道所有的aspect，这些东西聚合起来可以叫context
// aspect中可以包含q，相当于model（q）转成了不同的role：aspect/context
// data是直接调用aspect？显然他不需要，他需要把quoation转给自己的context或者quant，然后有这个人来调用，因为他知道所有的aspect---这个东西是啥，叫什么名字

struct Context0 : Context {
    void init() {
    }

private:
    Strategy*  _s;  // s 需要context->o
    Trader*    _t;
    Data*      _d;  // d 需要去调用_s;
    OrderMgmt* _o;  // o 需要调用t

    std::list<Aspect*> _aps;
};

int Quant::init() {
    Context0* ctx = new Context0();
    ctx->init();

    _d = ProxyFactory::create_data( 0 );
    _t = ProxyFactory::create_trader( 0 );

    return 0;
}

//--处理输入,命令等
int Quant::execute( Strategy* s_ ) {
    _s = s_;

    _s->on_init( _c );

    [[maybe_unused]] auto id = _timer.add(
        std::chrono::seconds( 2 ),
        [ & ]( cub::timer_id ) { this->ontick(); },
        std::chrono::seconds( 1 ) );

    for ( ;; ) {
        ::sleep( 1 );
    }

    return 0;
}

void Quant::quote( const quotation_t& q_ ) {
    _s->on_instant( q_ );

    

    _s->on_invoke( _c );
}

void Quant::update( const order_t& o_ ) {
}

void Quant::ontick() {
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
            DATA.start();
            TRADER.start();

            _working = true;
        }
    }

    if ( _working ) {
        auto diff = t.hour * 3600 - 60 * t.minute
                    - d.end.hour * 3600 + d.end.minute * 60;

        if ( diff > 5 * 60 ) {
            DATA.stop();
            TRADER.stop();
            _working = true;
        }
    }
}
CUB_NS_END