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

#include <chrono>
#include <cub.h>
#include <reactor.h>
#include <stdio.h>
#include <thread>
#include <unistd.h>

#include "../core/msg_int.h"

void foo( int a ) {
    int n = 10;
    while ( n-- >= 0 ) {
        fprintf( stderr, "-------\n" );
        ::sleep( 1 );
    }
}

int main() {

    cub::task_queue* q = cub::task_queue::create( 4 );

    q->run_r( foo, 1 );

    while ( q->busy() ) {
        ::sleep( 1 );
    }

    fprintf( stderr, "reactor test\n" );
    //---
    LOG_INIT_KEEP( "log_reactor.log", -1 );

    REACTOR.sub( { cub::msg::mid_t::data_tick }, []( const cub::msg::header_t& h ) {
        fprintf( stderr, "@" );
        // printf( "@@sub,code=%u, %d\n", ( unsigned )h.id, cub::msg::frame_cast<NVX_NS::msg::DataTickFrame>( h ).body.debug );
    } );

    REACTOR.sub( { cub::msg::mid_t::data_tick }, []( const cub::msg::header_t& h ) {
        fprintf( stderr, "~" );
        // printf( "&&sub,code=%u, %d\n", ( unsigned )h.id, cub::msg::frame_cast<NVX_NS::msg::DataTickFrame>( h ).body.debug );
    } );

    std::thread( [ & ]() {
        int seq = 0;
        for ( ;; ) {
            NVX_NS::msg::TickFrame d;

            if ( seq == 3 ) {
                REACTOR.sub( { cub::msg::mid_t::svc_data }, []( const cub::msg::header_t& h ) {
                    // printf( "svc+data code=%u\n", ( unsigned )h.id );
                    ::sleep( 5 );  // 测试一下水位设置，看看会不会丢消息
                    // printf( "###sub,code=%u, %d\n", ( unsigned )h.id, cub::msg::frame_cast<NVX_NS::msg::DataTickFrame>( h ).body.debug );
                } );
            }

            // fprintf( stderr, "pub---%d\n", d.body.debug );

            REACTOR.pub( d );

            d.h.id = cub::msg::mid_t::svc_data;

            // todo 如果inproc数据没有订阅者,这里会在send函数中卡住
            REACTOR.pub( d );
            std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
        }
    } ).join();

    return 0;
}