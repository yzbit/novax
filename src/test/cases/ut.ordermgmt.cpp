#include <core/order_mgmt.h>
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

struct FakeBroker : NVX_NS::IBroker {
    FakeBroker( NVX_NS::ITrader* t )
        : IBroker( t ) {}
#if 0
    int start() override {
        return 0;
    }

    int stop() {
        retturn 0;
    }
    virtual int put( const order_t& o_ )    = 0;
    virtual int cancel( const order_t& o_ ) = 0;
#endif

    // MOCK_METHOD( int, start, (), ( override ) );
    MOCK_METHOD0( start, int() );
    MOCK_METHOD0( stop, int() );
    MOCK_METHOD1( put, int( const NVX_NS::order_t& ) );
    MOCK_METHOD1( cancel, int( const NVX_NS::order_t& ) );
};

TEST( Mgmt, simpleput ) {
    NVX_NS::OrderMgmt m;
    FakeBroker        fb( &m );

    EXPECT_CALL( fb, start() ).WillOnce( testing::Return( 0 ) );
    EXPECT_CALL( fb, stop() ).WillOnce( testing::Return( 0 ) );
    EXPECT_CALL( fb, put( testing::_ ) ).Times( 1 ).WillRepeatedly( testing::Return( 0 ) );
    EXPECT_CALL( fb, cancel( testing::_ ) ).WillOnce( testing::Return( 0 ) );

    auto rc = m.start();
    rc      = m.stop();

    NVX_NS::order_t o;

    o.id = m.buylong( "rb2410", 1 );

    m.cancel( o.id );
    //  m.close( "aaaa" );
    // m.close( 1 );
}

TEST( Mgmt, putfail ) {
    NVX_NS::OrderMgmt m;
    FakeBroker        fb( &m );

    EXPECT_CALL( fb, put( testing::_ ) ).Times( 1 ).WillRepeatedly( testing::Return( -1 ) );

    auto id = m.buylong( "rb2410", 1 );
    ASSERT_EQ( id, NVX_NS::kBadId );
}

struct SimBroker : NVX_NS::IBroker {
    SimBroker( NVX_NS::ITrader* t )
        : NVX_NS::IBroker( t ) {}

    int start() override { return 0; }
    int stop() override { return 0; }
    int put( const NVX_NS::order_t& o_ ) override {

        if ( fail )
            delegator()->update_ord( o_.id, NVX_NS::ostatus_t::cancelled );
        else {
            if ( sell ) {
                o_.dir = NVX_NS::odir_t::sell;
            }
            else {
                o_.dir = NVX_NS::odir_t::buy;
            }
            delegator()->update_ord( o_ );
        }

        return 0;
    }

    int cancel( const NVX_NS::order_t& o_ ) override {
        return 0;
    }

    bool fail = true;
    bool sell = false;
};

TEST( Mgmt, position ) {
    NVX_NS::OrderMgmt m;

    SimBroker fb( &m );

    fb.flag = 0;
    auto id = m.buylong( "rb2410", 1 );
    ASSERT_EQ( id, NVX_NS::kBadId );

    fb.flag = 1;
    id      = m.buylong( "rb2410", 1 );
    ASSERT_GT( id, 0 );
}