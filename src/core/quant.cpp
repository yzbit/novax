/************************************************************************************
The MIT License

Copyright (c) 2024 YaoZinan  [zinan@outlook.com, nvx-quant.com]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

* \author: yaozn(zinan@outlook.com)
* \date: 2024
**********************************************************************************/

#include <future>
#include <unistd.h>

#include "quant.h"

#include "clock.h"
#include "concurrentqueue.h"
#include "context.h"
#include "contextimpl.h"
#include "data.h"
#include "ordermgmt.h"
#include "pub.h"
#include "strategy.h"

NVX_NS_BEGIN

struct quant_impl;

struct quant_pub : ipub {
    quant_pub( quant_impl& q );

    using con_queue = moodycamel::ConcurrentQueue<pub::msg>;

private:
    nvx_st post( const pub::msg& m_ ) override;
    nvx_st process();

private:
    static nvx_st thread_fun( quant_pub& pub_ );
    quant_impl&   quant() { return _q; }
    con_queue&    queue() { return _msg_q; }

private:
    bool             _running = false;
    con_queue        _msg_q;
    std::future<int> _fut;

private:
    quant_impl& _q;
};

struct quant_impl : quant {
    quant_impl();
    ~quant_impl();

private:
    nvx_st execute( strategy* s_ ) override;
    nvx_st invoke( strategy::ntf n_ ) {
        // return _s->invoke( n_, _c );
        return NVX_OK;
    }

private:
    static void thread_fun( quant_impl& q_ );

private:
    nvx_st on_tick( const pub::tick_msg& tick_ );
    nvx_st on_fund( const pub::acct_msg& tick_ );
    nvx_st on_error( const pub::error_msg& err_ );
    nvx_st on_order( const pub::order_msg& err_ );
    nvx_st on_position( const pub::pos_msg& err_ );
    nvx_st on_clock( const pub::timer_msg& err_ );

protected:
    data*         d();
    order_mgmt*   m();
    strategy*     s();
    context_intf* c();

private:
    data*         _data = nullptr;
    order_mgmt*   _mgmt = nullptr;
    strategy*     _s    = nullptr;
    context_intf* _ctx  = nullptr;

private:
    friend class quant_pub;
};

//----------
quant_pub::quant_pub( quant_impl& q_ )
    : _running{ true }
    , _q{ q_ } {
    // todo
    //  _fut = std::async( std::launch::async, &quant_pub::thread_fun, *this );
}

nvx_st quant_pub::process() {
    pub::msg m;

    uint64_t last = now_ms();
    for ( ;; ) {
        if ( now_ms() - last >= 1000 ) {
            post( pub::timer_msg() );
            last = now_ms();
        }

        if ( !_running ) break;

        if ( !queue().try_dequeue( m ) ) {
            std::this_thread::yield();
            continue;
        }

        switch ( m.type() ) {
        default: return _q.on_error( pub::error_msg() );
        case pub::msg_type::timer:
            return _q.on_clock( m.get<pub::timer_msg>() );
        case pub::msg_type::tick:
            // todo 如果后面还有一个tick（数据堆积了），最好是先处理完再调用invoke?
            return quant().on_tick( m.get<pub::tick_msg>() );
        case pub::msg_type::acct:
            return quant().on_fund( m.get<pub::acct_msg>() );
        case pub::msg_type::order:
            return quant().on_order( m.get<pub::order_msg>() );
        case pub::msg_type::error:
            return quant().on_error( m.get<pub::error_msg>() );
        case pub::msg_type::position:
            return quant().on_position( m.get<pub::pos_msg>() );
        }
    }

    return 0;
}

nvx_st quant_pub::post( const pub::msg& m_ ) {
    return queue().enqueue( m_ ) ? 0 : -1;
}

nvx_st quant_pub::thread_fun( quant_pub& pub_ ) {
    return pub_.process();
}

//-------quant_impl-------------------------------------
quant_impl::~quant_impl() {
    delete _data;
    delete _ctx;
    delete _mgmt;
    delete _s;
}

quant_impl::quant_impl() {
#if 0
    _data = new Data();
    _mgmt = new order_mgmt();
    _ctx  = new context_intf( _mgmt );
#endif
}

quant& quant::instance() {
    static quant_impl _impl;
    return _impl;
}

data*         quant_impl::d() { return _data; }
order_mgmt*   quant_impl::m() { return _mgmt; }
strategy*     quant_impl::s() { return _s; }
context_intf* quant_impl::c() { return _ctx; }

nvx_st quant_impl::execute( strategy* s_ ) {
    _s = s_;
    _s->init();
    for ( ;; )
        ::sleep( 1 );
    delete this;
    return NVX_OK;
}

nvx_st quant_impl::on_clock( const pub::timer_msg& err_ ) {
    return invoke( strategy::ntf::clock );
}

nvx_st quant_impl::on_tick( const pub::tick_msg& tick_ ) {
    d()->update( tick_ );
    c()->update_qut( tick_ );
    return invoke( strategy::ntf::tick );
}

nvx_st quant_impl::on_fund( const pub::acct_msg& fund_ ) {
    c()->update_fund( fund_ );
    return invoke( strategy::ntf::instate );
}

nvx_st quant_impl::on_error( const pub::error_msg& err_ ) {
#if 0
    context()->update_err( err_ );
    return invoke( strategy::ntf::error );
#endif
    return NVX_OK;
}

nvx_st quant_impl::on_order( const pub::order_msg& ord_ ) {
    m()->update_ord( ord_ );
    return invoke( strategy::ntf::order );
}

nvx_st quant_impl::on_position( const pub::pos_msg& err_ ) {
    return invoke( strategy::ntf::instate );
}

NVX_NS_END