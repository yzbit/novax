#include "data.h"

#include "aspect.h"
#include "log.hpp"
#include "proxy.h"
#include "quant_impl.h"
#include "reactor.h"
#include "strategy.h"

CUB_NS_BEGIN

Data::Data( QuantImpl* q_ )
    : _q( q_ ) {

    _jobs = TaskQueue::create( 4 );
    THREAD_DETACHED( [ & ]() { this->process(); } );
}

void Data::update( const quotation_t& tick_ ) {
    _cache.put( tick_ );
}

int Data::start() {
    return _d->start();
}

int Data::stop() {
    return _d->stop();
}

int Data::subscribe( const code_t& code_ ) {
    return _d->subscribe( code_ );
}

int Data::unsubscribe( const code_t& code_ ) {
    return _d->unsubscribe( code_ );
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
    quotation_t q;

    for ( ;; ) {
        if ( _cache.pop( q ) < 0 ) {
            std::this_thread::yield();
            continue;
        }

        _jobs->shutdown();

        _jobs->run( [ & ]() { _q->strategy()->on_instant( q ); } );

        for ( auto& as : _aspects ) {
            _jobs->run( [ & ]() { as->update( q ); } );
        }

        while ( _jobs->busy() ) {
            std::this_thread::yield();
        }

        _q->strategy()->on_invoke( 0 );
    }
}

Data::~Data() {
    delete _d;

    _jobs->shutdown();
    delete _jobs;
    for ( auto as : _aspects ) {
        delete as;
    }
}

CUB_NS_END
