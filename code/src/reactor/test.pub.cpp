#include <comm/log.hpp>
#include <reactor/reactor.h>
#include <stdio.h>
#include <thread>
#include <unistd.h>

int main() {
    LOG_INIT( "log_reactor", -1 );

    REACTOR.init();

    REACTOR.sub( {1024 }, []( const CUB_NS::Msg& m ) {
        // REACTOR.sub( { }, []( const CUB_NS::Msg& m ) {
        printf( "sub,code=%u\n", m.id );
    } );

    std::thread( []() {
        for ( ;; ) {
            fprintf( stderr, "pub\n" );
            CUB_NS::Msg m = { 0 };
            //strcpy( m.topic, "abc" );
            m.id = 1024;
            REACTOR.pub( m );
            ::sleep( 1 );
        }
    } ).join();

    return 0;
}