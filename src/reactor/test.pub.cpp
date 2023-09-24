#include <comm/log.hpp>
#include <reactor/reactor.h>
#include <stdio.h>
#include <thread>
#include <unistd.h>

int main() {
    LOG_INIT_KEEP( "log_reactor.log", -1 );

    // REACTOR.init();

    REACTOR.sub( { cub::msg::mid_t::data_tick }, []( const cub::msg::Header& h ) {
        // REACTOR.sub( { }, []( const CUB_NS::Msg& m ) {
        printf( "sub,code=%u\n", ( unsigned )h.id );
    } );

    std::thread( []() {
        for ( ;; ) {
            fprintf( stderr, "pub\n" );
            CUB_NS::msg::DataTickFrame d;
            // strcpy( m.topic, "abc" );
            d.h.id = cub::msg::mid_t::data_tick;

            REACTOR.pub( d );
            ::sleep( 1 );
        }
    } ).join();

    return 0;
}