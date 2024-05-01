#include <gtest/gtest.h>
#include <novax.h>
#include <stdio.h>

TEST( Series, Basic ) {
    nvx::Series<int> s( 5 );

    ASSERT_EQ( s.size(), 1 );

    s.shift();

    ASSERT_EQ( s.size(), 2 );

    // s.for_each( []( auto& e_ ) -> bool {
    s.for_each( [ & ]( auto& e_ ) -> bool {
        // ASSERT_EQ( e_, 0 );
        assert( e_ == 0 );
        printf( "%d\n", e_ );
        return true;
    } );

    s.update( 0, 5 );
    ASSERT_EQ( s[ 0 ], 5 );
    ASSERT_EQ( s.get( 0 ), 5 );
    ASSERT_EQ( s.at( 0 ), 5 );

    s.at( 0 ) = 6;
    ASSERT_EQ( s[ 0 ], 6 );
}
