#include "data.h"

#include "aspect.h"
#include "dci_role.h"
#include "log.hpp"
#include "proxy.h"
#include "quant_impl.h"
#include "reactor.h"
#include "strategy.h"

NVX_NS_BEGIN

Data& Data::instance() {
    static Data d;
    return d;
}

Data::Data() {
    _m    = create_market( this );
    _jobs = TaskQueue::create( 4 );

    THREAD_DETACHED( [ & ]() { this->process(); } );
}

void Data::update( const quotation_t& tick_ ) {
    _cache.put( tick_ );

    std::unique_lock<std::mutex> lock{ _mutex };
    _cv.notify_one();
}

int Data::start() {
    return _m->start();
}

int Data::stop() {
    return _m->stop();
}

int Data::subscribe( const code_t& code_ ) {
    return _m->subscribe( code_ );
}

int Data::unsubscribe( const code_t& code_ ) {
    return _m->unsubscribe( code_ );
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

        _jobs->run( [ & ]() { _r->update( q ); } );

        for ( auto& as : _aspects ) {
            _jobs->run( [ & ]() { as->update( q ); } );
        }

        while ( _jobs->busy() ) {
            std::this_thread::yield();
        }

        _r->invoke();
    }
}

Data::~Data() {
    delete _m;

    _jobs->shutdown();
    delete _jobs;
    for ( auto as : _aspects ) {
        delete as;
    }
}

NVX_NS_END
