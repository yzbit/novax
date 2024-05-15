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
#include <thread>

#include "pub.h"

#include "concurrentqueue.h"

NVX_NS_BEGIN

struct QuantPub : IPub {
    QuantPub();

private:
    int post( const msg_t& m_ ) override;

private:
    static int thread_fun( QuantPub& pub_ );
    int        process();
    int        on_tick( const tick_msg_t& tick_ );
    int        on_fund( const fund_msg_t& tick_ );
    int        on_error( const error_msg_t& err_ );
    int        on_order( const order_msg_t& err_ );
    int        on_position( const pos_msg_t& err_ );

private:
    moodycamel::ConcurrentQueue<pub::msg_t> _q;

    bool             _running = false;
    std::future<int> _fut;
};

//--
QuantPub::QuantPub() {
    _running = true;
    _fut     = std::async( std::launch::async, &QuantPub::thread_fun, *this );
}

int QuantPub::process() {
    msg_t m;
    for ( ;; ) {
        if ( !_running ) break;
        if ( !_q.try_dequeue( m ) ) {
            std::this_thread::yield();
        }
        else {
            switch ( m.type() ) {
            default: return on_error( error_msg_t() );
            case msg_type::tick:
                return on_tick( m.get<tick_msg_t>() );
            case msg_type::fund:
                return on_fund( m.get<fund_msg_t>() );
            case msg_type::order:
                return on_order( m.get<order_msg_t>() );
            case msg_type::error:
                return on_error( m.get<error_msg_t>() );
            case msg_type::position:
                return on_position( m.get<pos_msg_t>() );
            }
        }
    }
    return 0;
}

int QuantPub::post( const msg_t& m_ ) {
    return _q.enqueue( m_ ) ? 0 : -1;
}

int QuantPub::thread_fun( QuantPub& pub_ ) {
    return pub_.process();
}

int QuantPub::on_tick( const tick_msg_t& tick_ ) {
    return 0;
}

int QuantPub::on_fund( const fund_msg_t& tick_ ) {
    return 0;
}

int QuantPub::on_error( const error_msg_t& err_ ) {
    return 0;
}

int QuantPub::on_order( const order_msg_t& err_ ) {
    return 0;
}

int QuantPub::on_position( const pos_msg_t& err_ ) {
    return 0;
}

NVX_NS_END