#include <gtest/gtest.h>
#include <novax.h>
#include <stdio.h>

TEST( Calendar, Basic ) {
    nvx::Calendar c;
    Calendar::load_schedule(nullptr);
    ASSERT_FALSE( c.is_trade_day() );

  
}
