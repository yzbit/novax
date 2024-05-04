#include <gtest/gtest.h>
#include <novax.h>
#include <stdio.h>
#include <time.h>

void foo( NVX_NS::exid_t id_, const NVX_NS::datetime_t& dt_ ) {
    ASSERT_TRUE( id_ == -1 || id_ == 2 );

    if ( id_ == -1 )
        dt_.print( "default:" );
    else
        dt_.print( "2:" );
}

TEST( Clock, Basic ) {
    // NVX_NS::Clock c;
    ASSERT_LE( CLOCK.now() - time( 0 ), 1 );

    printf( "now=%ld\n", CLOCK.now() );

    CLOCK.tune( NVX_NS::datetime_t().from_unix_time( time( 0 ) + 5 ), 2 );
    ASSERT_GE( CLOCK.now( 2 ) - time( 0 ), 4 );
    printf( "exid=2,drift=%ld\n", CLOCK.now( 2 ) - time( 0 ) );

    CLOCK.attach( foo );
    CLOCK.attach( foo, 2 );

    ::sleep( 3 );
}