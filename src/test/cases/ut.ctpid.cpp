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
#include <core/ctp/ctpids.h>
#include <gtest/gtest.h>
#include <novax.h>

USE_NVX_NS

TEST( REF, basic ) {
    ctp::ref_t r( 168 );

    ASSERT_TRUE( r.int_val() == 168 );
    ASSERT_TRUE( strlen( r.data() ) == strlen( "1234567890123" ) );

    ASSERT_TRUE( strcmp( "0000000000168", r.data() ) == 0 );

    ++r;
    ASSERT_TRUE( strcmp( "0000000000169", r.data() ) == 0 );

    r += 1000;
    ASSERT_TRUE( strcmp( "0000000001169", r.data() ) == 0 );

    ctp::ref_t r2( r );
    ASSERT_TRUE( strcmp( "0000000001169", r2.data() ) == 0 );

    ctp::ref_t r3;
    ASSERT_TRUE( strcmp( "0000000000000", r3.data() ) == 0 );
    r3 = r2;

    ASSERT_TRUE( strcmp( "0000000001169", r3.data() ) == 0 );

    ctp::ref_t r4 = r3 + 1;
    ASSERT_TRUE( strcmp( "0000000001170", r4.data() ) == 0 );

    TThostFtdcOrderRefType ref;
    r4.copy_to( ref );
    ASSERT_TRUE( strncmp( "0000000001170", ref, 13 ) == 0 );

    ASSERT_TRUE( r4 == 1170 );
}

TEST( IdMgr, basic ) {
    TThostFtdcExchangeIDType ex = { 'a' };
    TThostFtdcOrderSysIDType id = { 'b' };

    NVX_NS::ctp::fsr_t   fsr;
    NVX_NS::ctp::ctpex_t eid( ex );
    NVX_NS::ctp::exoid_t oid( id );

    NVX_NS::ctp::IdMgr m;

    fsr.ref = 99;

    m.insert( { fsr, eid, oid } );

    ASSERT_TRUE( m.id_of( 99 ).value().fsr.ref.int_val() == 99 );
    ASSERT_TRUE( m.id_of( fsr ).value().fsr.ref.int_val() == 99 );
    ASSERT_TRUE( m.id_of( eid, oid ).value().fsr.ref.int_val() == 99 );

    m.remove( 99 );
    ASSERT_FALSE( m.id_of( 99 ).has_value() );

    m.insert( { fsr, eid, oid } );
    m.remove( fsr );
    ASSERT_FALSE( m.id_of( 99 ).has_value() );

    m.insert( { fsr, eid, oid } );

    ex[ 0 ] = 'x';
    id[ 0 ] = 'y';
    m.update_sysid( fsr, ex, id );
    ASSERT_EQ( m.id_of( 99 ).value().ex.data()[ 0 ], 'x' );

    NVX_NS::ctp::ctpex_t eid2;
    eid2 = ex;
    ASSERT_EQ( eid2.data()[ 0 ], 'x' );

    ASSERT_TRUE( eid2 == ex );
    ASSERT_FALSE( eid2 != ex );

    ASSERT_TRUE( eid2.is_valid() );

    NVX_NS::ctp::ctpex_t eid3;
    ASSERT_FALSE( eid3.is_valid() );

    NVX_NS::ctp::order_id_t id0{ fsr, ex, id };

    ASSERT_TRUE( id0.is_valid() );
    ASSERT_TRUE( id0.has_refid() );
    ASSERT_TRUE( id0.has_sysid() );

    NVX_NS::ctp::order_id_t id1{ fsr, NVX_NS::ctp::ctpex_t(), NVX_NS::ctp::exoid_t() };

    ASSERT_TRUE( id1.is_valid() );
    ASSERT_TRUE( id1.has_refid() );
    ASSERT_FALSE( id1.has_sysid() );
}

#endif /* C1DE8F61_B7D0_4C2B_82B7_DEB95AACC4B7 */
