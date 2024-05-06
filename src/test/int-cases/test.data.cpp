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
#include <stdio.h>

#include "../core/data.h"
#include "../core/dci_role.h"
#include "../core/log.hpp"
#include "../core/models.h"

struct XContext : cub::DataContext {
    void update( const cub::quotation_t& q_ ) {
        std::cout << "update: q.ask=" << q_.ask << std::endl;
    }

    void invoke() {
        std::cout << "invoke" << std::endl;
    }
};

int main() {
    LOG_INIT( "./log/testasp", -1 );
    LOG_ENABLE_STDOUT();

    cub::quotation_t q;
    q.ask = 99.99;

    auto d = new cub::Data( new XContext() );
    d->start();
    d->stop();

    d->subscribe( "rb2401" );
    // d->unsubscribe( "rb2410" );

    d->update( q );

    fprintf( stderr, "case finish\n" );

    for ( int i = 0; i < 10; ++i ) {
        fprintf( stderr, "-\n" );
        ::sleep( 1 );
    }
    return 0;
}