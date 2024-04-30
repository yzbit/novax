#include "data.h"

#include "aspect.h"
#include "context.h"
#include "log.hpp"
#include "proxy.h"
#include "quant.h"
#include "strategy.h"

NVX_NS_BEGIN

Data::Data( Quant* q_ )
    : _q( q_ ) {
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
    return market->stop();
}

int Data::subscribe( const code_t& code_ ) {
    return market->subscribe( code_ );
}

int Data::unsubscribe( const code_t& code_ ) {
    return market->unsubscribe( code_ );
}

int Data::attach( Aspect* a_ ) {
    _aspects.push_back( a_ );
    return 0;
}

Aspect* Data::attach( const code_t& symbol_, const period_t& period_, int count_ ) {
    if ( subscribe( symbol_ ) < 0 )
        return nullptr;

    Aspect* a = new Aspect();
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

        _jobs->run( [ & ]() { _q->context()->update_qut( q ) } );

        for ( auto& as : _aspects ) {
            _jobs->run( [ & ]() { as->update( q ); } );
        }

        while ( _jobs->busy() ) {
            std::this_thread::yield();
        }

        _q->invoke();
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
