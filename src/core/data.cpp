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

#include "data.h"

#include "aspect.h"
#include "context.h"
#include "log.hpp"
#include "proxy.h"
#include "quant.h"
#include "strategy.h"

NVX_NS_BEGIN

Data::Data() {
    _jobs = TaskQueue::create( 4 );

    THREAD_DETACHED( [ & ]() { this->process(); } );
}

void Data::update( const quotation_t& tick_ ) {
    _cache.put( tick_ );

    std::unique_lock<std::mutex> lock{ _mutex };
    _cv.notify_one();
}

int Data::start() {
    return market()->start();
}

int Data::stop() {
    return market()->stop();
}

int Data::attach( Aspect* a_ ) {
    _aspects.push_back( a_ );

    return 0;
}

int Data::dettach( Aspect* a_ ) {
    if ( !a_ ) return -1;
    return market()->unsubscribe( a_->code() );
}

Aspect* Data::attach( const code_t& symbol_, const period_t& period_, int count_ ) {
    if ( market()->subscribe( symbol_ ) < 0 )
        return nullptr;

    Aspect* a = new Aspect( this );
    a->load( symbol_, period_, count_ );

    attach( a );
    return a;
}

void Data::process() {
    quotation_t q = { 0 };

    auto wait = [ & ]() {
        std::unique_lock<std::mutex> lock{ _mutex };
        _cv.wait( lock, [ & ]() {
            return _cache.size() > 0;
        } );
    };

    for ( ;; ) {
        wait();

        if ( _cache.pop( q ) < 0 ) {
            std::this_thread::yield();
            continue;
        }

        _jobs->drain();

        _jobs->run( [ & ]() { QUANT.context()->update_qut( q ); } );

        for ( auto& as : _aspects ) {
            _jobs->run( [ & ]() { as->update( q ); } );
        }

        while ( _jobs->busy() ) {
            std::this_thread::yield();
        }

        QUANT.invoke();
    }
}

Data::~Data() {
    _jobs->shutdown();
    delete _jobs;
    for ( auto as : _aspects ) {
        delete as;
    }
}

NVX_NS_END
