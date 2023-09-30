#include <chrono>
#include <cub.h>
#include <reactor/reactor.h>
#include <stdio.h>
#include <thread>
#include <unistd.h>

void foo( int a ) {
    int n = 10;
    while ( n-- >= 0 ) {
        fprintf( stderr, "-------\n" );
        ::sleep( 1 );
    }
}

int main() {

    cub::TaskQueue* q = cub::TaskQueue::create( 4 );

    q->run_r( foo, 1 );

    while ( q->busy() ) {
        ::sleep( 1 );
    }

    fprintf( stderr, "reactor test\n" );
    //---
    LOG_INIT_KEEP( "log_reactor.log", -1 );

    REACTOR.sub( { cub::msg::mid_t::data_tick }, []( const cub::msg::header_t& h ) {
        fprintf( stderr, "@" );
        // printf( "@@sub,code=%u, %d\n", ( unsigned )h.id, cub::msg::frame_cast<CUB_NS::msg::DataTickFrame>( h ).body.debug );
    } );

    REACTOR.sub( { cub::msg::mid_t::data_tick }, []( const cub::msg::header_t& h ) {
        fprintf( stderr, "~" );
        // printf( "&&sub,code=%u, %d\n", ( unsigned )h.id, cub::msg::frame_cast<CUB_NS::msg::DataTickFrame>( h ).body.debug );
    } );

    std::thread( [ & ]() {
        int seq = 0;
        for ( ;; ) {
            CUB_NS::msg::QuotationFrame d;

            if ( seq == 3 ) {
                REACTOR.sub( { cub::msg::mid_t::svc_data }, []( const cub::msg::header_t& h ) {
                    // printf( "svc+data code=%u\n", ( unsigned )h.id );
                    ::sleep( 5 );  // 测试一下水位设置，看看会不会丢消息
                    // printf( "###sub,code=%u, %d\n", ( unsigned )h.id, cub::msg::frame_cast<CUB_NS::msg::DataTickFrame>( h ).body.debug );
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