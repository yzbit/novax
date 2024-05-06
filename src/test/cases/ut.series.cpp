/************************************************************************************
The MIT License

Copyright (c) 2024 YaoZinan  [zinan@outlook.com, nvx-quant.com]

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

* \author: yaozn(zinan@outlook.com)
* \date: 2024
**********************************************************************************/

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

TEST( Series, Adv ) {
    nvx::Series<int> s( 3 );

    for ( int i = 0; i < 8; i++ ) {
        s.append( i );
    }

    ASSERT_EQ( s.size(), 3 );
    ASSERT_EQ( s[ 0 ], 7 );
    ASSERT_EQ( s[ 1 ], 6 );
    ASSERT_EQ( s[ 2 ], 5 );
    ASSERT_EQ( s.current(), 7 );
    ASSERT_EQ( s.append( 9 ), 9 );
}
