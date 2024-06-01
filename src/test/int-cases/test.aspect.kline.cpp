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

#include <iostream>

#include "../core/aspect.h"
#include "../core/definitions.h"
#include "../core/indicator.h"
#include "../core/kline.h"
#include "../core/log.hpp"

struct TestI : cub::Indicator {
    cub::xstring name() override { return "#test-indicator"; }

    void on_calc( const cub::tick& q_ ) override {
        std::cout << "update q with ask=" << q_.ask << std::endl;
    }
};

int main() {
    LOG_INIT( "./log/testasp", -1 );
    LOG_ENABLE_STDOUT();

    auto* a = new cub::aspect();
    a->load( "rb2410", cub::period{ cub::period::type_t::min, 1 }, 40 );
    auto i = new TestI();
    a->addi( i );
    a->debug();

    //---
    cub::tick q;
    q.ask = 99.99;

    a->update( q );

    auto& b = a->kline().bar();

    std::cout << "first bar's price=" << b.price();

    return 0;
}