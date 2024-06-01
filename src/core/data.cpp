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

#include "data.h"

#include "aspect.h"
#include "context.h"
#include "log.hpp"
#include "proxy.h"
#include "quant.h"
#include "strategy.h"

NVX_NS_BEGIN

data::data( market* market_ )
    : _market( market_ ) {
    _jobs = task_queue::create( 4 );
}

void data::update( const tick& tick_ ) {
    _jobs->drain();

    for ( auto& as : _aspects ) {
        _jobs->run( [ & ]() { as->update( tick_ ); } );
    }

    while ( _jobs->busy() ) {
        std::this_thread::yield();
    }
}

nvx_st data::start() {
    return _market->start();
}

nvx_st data::stop() {
    return _market->stop();
}

nvx_st data::attach( aspect* a_ ) {
    _aspects.push_back( a_ );

    return 0;
}

nvx_st data::dettach( aspect* a_ ) {
    if ( !a_ ) return NVX_FAIL;
    return _market->unsubscribe( a_->code() );
}

aspect* data::attach( const code& symbol_, const Period& period_, int count_ ) {
    if ( _market->subscribe( symbol_ ) < 0 )
        return nullptr;

    aspect* a = new aspect( this );
    a->load( symbol_, period_, count_ );

    attach( a );
    return a;
}

data::~Data() {
    _jobs->shutdown();
    delete _jobs;
    for ( auto as : _aspects ) {
        delete as;
    }
}

NVX_NS_END
