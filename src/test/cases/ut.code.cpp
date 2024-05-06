#include <gtest/gtest.h>
#include <novax.h>
#include <set>
#include <stdio.h>

TEST( Code_t, basic ) {
    NVX_NS::code_t c;

    ASSERT_FALSE( c );
    ASSERT_TRUE( c.empty() );
    ASSERT_EQ( c.length(), 0 );
}

TEST( Code_t, eq ) {
    NVX_NS::code_t c1( "rb24123456" );
    ASSERT_TRUE( ( c1 == "rb24123" ) );

    NVX_NS::code_t c2( "rb24123" );
    ASSERT_EQ( c1, c2 );
    ASSERT_EQ( c1.length(), 7 );
}

TEST( Code_t, inset ) {
    std::set<NVX_NS::code_t> codes{ "rb2410", "y2509" };
    ASSERT_TRUE( codes.count( "rb2410" ) > 0 );
}

TEST( Code_t, conv ) {
    NVX_NS::code_t c2( "rb2410" );

    ASSERT_TRUE( 0 == strcmp( "rb2410", c2.c_str() ) );
    ASSERT_TRUE( c2 );
    ASSERT_TRUE( code2ins( c2 ) == "rb" );
}