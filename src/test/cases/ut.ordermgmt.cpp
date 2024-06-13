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

#include <core/ord_mgmt.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <novax.h>

/**
    TEST(TestForMyClass, TestRetVal)
    {
    FakeMyClass obj3;
    EXPECT_CALL(obj3, retValue()).WillOnce(Return(3));
    EXPECT_EQ(obj3.retValue(), 3);
    }
    If you still believe, that you want to call implementation from class MyClass, you can always do something like that:

    TEST(TestForMyClass, TestRetVal)
    {
    FakeMyClass obj3;
    EXPECT_CALL(obj3, retValue()).WillOnce(Return(obj3.MyClass::retValue()));
    EXPECT_EQ(obj3.retValue(), 3);
    }

    Finally, if you don't really care how many times FakeMyClass is called and you just want to return "3" whenever it happens, you can use NiceMock:

    TEST(TestForMyClass, TestRetVal)
    {
    FakeMyClass obj3;
    NiceMock<FakeMyClass> niceMockForFakeClass;
    ON_CALL(niceMockForFakeClass, retValue()).WillByDefault(Return(3));
    EXPECT_EQ(niceMockForFakeClass.retValue(), 3);
}
*/

struct FakeBroker : NVX_NS::broker {
    FakeBroker( NVX_NS::ITrader* t )
        : broker( t ) {}
#if 0
    int start() override {
        return 0;
    }

    int stop() {
        retturn 0;
    }
    virtual int put( const order& o_ )    = 0;
    virtual int cancel( const order& o_ ) = 0;
#endif

    // MOCK_METHOD( int, start, (), ( override ) );
    MOCK_METHOD0( start, int() );
    MOCK_METHOD0( stop, int() );
    MOCK_METHOD1( put, int( const NVX_NS::order& ) );
    MOCK_METHOD1( cancel, int( const NVX_NS::order& ) );
};

TEST( Mgmt, simpleput ) {
    NVX_NS::ord_mgmt m;
    FakeBroker        fb( &m );

    EXPECT_CALL( fb, start() ).WillOnce( testing::Return( 0 ) );
    EXPECT_CALL( fb, stop() ).WillOnce( testing::Return( 0 ) );
    EXPECT_CALL( fb, put( testing::_ ) ).Times( 1 ).WillRepeatedly( testing::Return( 0 ) );
    EXPECT_CALL( fb, cancel( testing::_ ) ).WillOnce( testing::Return( 0 ) );

    auto rc = m.start();
    rc      = m.stop();

    NVX_NS::order o;

    o.id = m.buylong( "rb2410", 1 );

    m.cancel( o.id );
    //  m.close( "aaaa" );
    // m.close( 1 );
}

TEST( Mgmt, putfail ) {
    NVX_NS::ord_mgmt m;
    FakeBroker        fb( &m );

    EXPECT_CALL( fb, put( testing::_ ) ).Times( 1 ).WillRepeatedly( testing::Return( -1 ) );

    auto id = m.buylong( "rb2410", 1 );
    ASSERT_EQ( id, NVX_NS::NVX_BAD_OID );
}

/// -------------------------------------------------
struct SimBroker : NVX_NS::broker {
    SimBroker( NVX_NS::ITrader* t )
        : NVX_NS::broker( t ) {}

    int start() override { return 0; }
    int stop() override { return 0; }
    int put( const NVX_NS::order& o_ ) override {
        delegator()->update_ord( o_.id, NVX_NS::ord_status::cancelled );
        return 0;
    }

    int cancel( const NVX_NS::order& o_ ) override {
        return 0;
    }

    void update( NVX_NS::oid id, int flag ) {
        // delegator()->update_ord( o_ );
    }
};

TEST( Mgmt, position ) {
    NVX_NS::ord_mgmt m;

    SimBroker fb( &m );

    auto id = m.buylong( "rb2410", 1 );
    ASSERT_EQ( id, NVX_NS::NVX_BAD_OID );

    fb.update( 0, 0 );
}