#include <comm/log.hpp>
#include <reactor/reactor.h>
#include <stdio.h>
#include <thread>
#include <unistd.h>

int main() {
    LOG_INIT_KEEP( "log_reactor.log", -1 );


//如果下面两个sub都开启,后面的pub无论那种消息都会被第二个sub拦截
//如果只开第一个sub,但是却pub 了两种消息,则会卡在第二个pub上  


#if 1
    REACTOR.sub( { cub::msg::mid_t::data_tick }, []( const cub::msg::Header& h ) {
        printf( "@@sub,code=%u, %d\n", ( unsigned )h.id, cub::msg::frame_cast<CUB_NS::msg::DataTickFrame>( h ).body.debug );
    } );
#endif

  REACTOR.sub( { cub::msg::mid_t::svc_data }, []( const cub::msg::Header& h ) {
        // printf( "svc+data code=%u\n", ( unsigned )h.id );
        printf( "###sub,code=%u, %d\n", ( unsigned )h.id, cub::msg::frame_cast<CUB_NS::msg::DataTickFrame>( h ).body.debug );
    } );

#if 0
    std::thread( [ & ]() {
        int seq = 10000;
        for ( ;; ) {
            CUB_NS::msg::DataTickFrame d;
            d.body.debug = seq++;

            fprintf( stderr, "pub---%d\n", d.body.debug );
            REACTOR.pub( d );

            d.h.id = cub::msg::mid_t::svc_data;
            REACTOR.pub( d );
            ::sleep( 2 );
        }
    } ).detach();
#endif

    std::thread( [ & ]() {
        int seq = 0;
        for ( ;; ) {
            CUB_NS::msg::DataTickFrame d;
            d.h.id       = cub::msg::mid_t::data_tick;
            d.body.debug = seq;

            if ( ++seq >= 15 ) {
                fprintf( stderr, "-------\n" );
                ::sleep( 1 );
                continue;
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