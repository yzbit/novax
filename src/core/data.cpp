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
