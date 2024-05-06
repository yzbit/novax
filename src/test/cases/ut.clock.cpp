/************************************************************************************
MIT License

Copyright (c) 2024 [YaoZinan zinan@outlook.com nvx-quant.com]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*@init: Yaozn
*@contributors:Yaozn
*@update: 2024
**********************************************************************************/

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