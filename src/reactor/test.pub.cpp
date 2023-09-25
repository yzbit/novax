#include <comm/log.hpp>
#include <reactor/reactor.h>
#include <stdio.h>
#include <thread>
#include <unistd.h>

int main() {
    LOG_INIT_KEEP( "log_reactor.log", -1 );

    // REACTOR.init();
    REACTOR.sub( { cub::msg::mid_t::data_tick }, []( const cub::msg::Header& h ) {
        printf( "sub,code=%u, %d\n", ( unsigned )h.id, cub::msg::frame_cast<CUB_NS::msg::DataTickFrame>( h ).body.debug );
    } );

#if 0
    REACTOR.sub( { cub::msg::mid_t::svc_data }, []( const cub::msg::Header& h ) {
        printf( "svc+data code=%u\n", ( unsigned )h.id );
    } );
#endif

#if 0
    CUB_NS::msg::DataTickFrame d;
    // strcpy( m.topic, "abc" );
    d.h.id = cub::msg::mid_t::data_tick;

    REACTOR.pub( d );
#endif
    // d.h.id = cub::msg::mid_t::svc_data;
    // REACTOR.pub( d );

    int seq = 0;
    std::thread( [ & ]() {
        for ( ;; ) {
            fprintf( stderr, "pub---1\n" );
            CUB_NS::msg::DataTickFrame d;
            // strcpy( m.topic, "abc" );
            d.h.id       = cub::msg::mid_t::data_tick;
            d.body.debug = seq;

            if ( ++seq >= 15 ) {
                fprintf( stderr, "-------\n" );
                ::sleep( 1 );
                continue;
            }

            fprintf( stderr, "pub---2\n" );
            REACTOR.pub( d );

            fprintf( stderr, "pub---3\n" );
            // d.h.id = cub::msg::mid_t::svc_data;
            //  REACTOR.pub( d );
            //::usleep( 300 );
            ::sleep( 1 );
        }
    } ).join();

    return 0;
}