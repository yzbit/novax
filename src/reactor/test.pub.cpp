#include <comm/log.hpp>
#include <reactor/reactor.h>
#include <stdio.h>
#include <thread>
#include <unistd.h>

int main() {
    LOG_INIT_KEEP( "log_reactor.log", -1 );

    // REACTOR.init();
#if 0
    REACTOR.sub( { cub::msg::mid_t::data_tick }, []( const cub::msg::Header& h ) {
        printf( "sub,code=%u\n", ( unsigned )h.id );
    } );
#endif

    REACTOR.sub( { cub::msg::mid_t::svc_data }, []( const cub::msg::Header& h ) {
        printf( "sub,code=%u\n", ( unsigned )h.id );
    } );

    std::thread( []() {
        for ( ;; ) {
            fprintf( stderr, "pub\n" );
            CUB_NS::msg::DataTickFrame d;
            // strcpy( m.topic, "abc" );
            d.h.id = cub::msg::mid_t::data_tick;

            //REACTOR.pub( d );

            d.h.id = cub::msg::mid_t::svc_data;
            REACTOR.pub( d );

            ::sleep( 1 );
        }
    } ).join();

    return 0;
}