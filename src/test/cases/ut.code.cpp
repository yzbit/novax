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