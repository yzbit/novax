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

#ifndef C1DE8F61_B7D0_4C2B_82B7_DEB95AACC4B7
#define C1DE8F61_B7D0_4C2B_82B7_DEB95AACC4B7
#include <gtest/gtest.h>
#include <novax.h>

#include "../../core/ctp/ctp_ids.h"
USE_NVX_NS

TEST( REF, basic ) {
    ctp::order_ref_t r( 168 );

    ASSERT_TRUE( r.int_val() == 168 );
    ASSERT_TRUE( strlen( r.str_val() ) == strlen( "1234567890123" ) );

    ASSERT_TRUE( strcmp( "0000000000168", r.str_val() ) == 0 );

    ++r;
    ASSERT_TRUE( strcmp( "0000000000169", r.str_val() ) == 0 );

    r += 1000;
    ASSERT_TRUE( strcmp( "0000000001169", r.str_val() ) == 0 );

    ctp::order_ref_t r2( r );
    ASSERT_TRUE( strcmp( "0000000001169", r2.str_val() ) == 0 );

    ctp::order_ref_t r3;
    ASSERT_TRUE( strcmp( "0000000000000", r3.str_val() ) == 0 );
    r3 = r2;

    ASSERT_TRUE( strcmp( "0000000001169", r3.str_val() ) == 0 );

    ctp::order_ref_t r4 = r3 + 1;
    ASSERT_TRUE( strcmp( "0000000001170", r4.str_val() ) == 0 );

    TThostFtdcOrderRefType ref;
    r4.copy( ref );
    ASSERT_TRUE( strncmp( "0000000001170", ref, 13 ) == 0 );

    ASSERT_TRUE( r4 == 1170 );
}

#endif /* C1DE8F61_B7D0_4C2B_82B7_DEB95AACC4B7 */
