#include <comm/log.hpp>
#include <reactor/reactor.h>
#include <stdio.h>
#include <thread>
#include <unistd.h>

int main() {
    LOG_INIT_KEEP( "log_reactor.log", -1 );

    REACTOR.sub( { cub::msg::mid_t::data_tick }, []( const cub::msg::Header& h ) {
        printf( "@@sub,code=%u, %d\n", ( unsigned )h.id, cub::msg::frame_cast<CUB_NS::msg::DataTickFrame>( h ).body.debug );
    } );

    REACTOR.sub( { cub::msg::mid_t::data_tick }, []( const cub::msg::Header& h ) {
        printf( "&&sub,code=%u, %d\n", ( unsigned )h.id, cub::msg::frame_cast<CUB_NS::msg::DataTickFrame>( h ).body.debug );
    } );

    std::thread( [ & ]() {
        int seq = 0;
        for ( ;; ) {
            CUB_NS::msg::DataTickFrame d;
            d.h.id       = cub::msg::mid_t::data_tick;
            d.body.debug = ++seq;

            if ( seq == 3 ) {
                REACTOR.sub( { cub::msg::mid_t::svc_data }, []( const cub::msg::Header& h ) {
                    // printf( "svc+data code=%u\n", ( unsigned )h.id );
                    printf( "###sub,code=%u, %d\n", ( unsigned )h.id, cub::msg::frame_cast<CUB_NS::msg::DataTickFrame>( h ).body.debug );
                } );
            }

            fprintf( stderr, "pub---%d\n", d.body.debug );

            REACTOR.pub( d );

            d.h.id = cub::msg::mid_t::svc_data;

            // todo 如果inproc数据没有订阅者,这里会在send函数中卡住
            REACTOR.pub( d );
            ::sleep( 1 );
        }
    } ).join();

    return 0;
}