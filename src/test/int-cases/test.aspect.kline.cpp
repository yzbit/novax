#include <iostream>

#include "../core/aspect.h"
#include "../core/definitions.h"
#include "../core/indicator.h"
#include "../core/kline.h"
#include "../core/log.hpp"

struct TestI : cub::Indicator {
    cub::string_t name() override { return "#test-indicator"; }

    void on_calc( const cub::quotation_t& q_ ) override {
        std::cout << "update q with ask=" << q_.ask << std::endl;
    }
};

int main() {
    LOG_INIT( "./log/testasp", -1 );
    LOG_ENABLE_STDOUT();

    auto* a = new cub::Aspect();
    a->load( "rb2410", cub::period_t{ cub::period_t::type_t::min, 1 }, 40 );
    auto i = new TestI();
    a->addi( i );
    a->debug();

    //---
    cub::quotation_t q;
    q.ask = 99.99;

    a->update( q );

    auto& b = a->kline().bar();

    std::cout << "first bar's price=" << b.price();

    return 0;
}