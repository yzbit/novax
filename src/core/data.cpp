#include "data.h"

#include "aspect.h"
#include "log.hpp"
#include "proxy.h"
#include "quant.h"
#include "reactor.h"
#include "strategy.h"

CUB_NS_BEGIN

Data::Data( Quant* q_ )
    : _q( q_ ) {

    _jobs = TaskQueue::create( 4 );
    THREAD_DETACHED( [ & ]() { this->process(); } );
}

void Data::process() {
    quotation_t q;

    for ( ;; ) {
        if ( _cache.pop( q ) < 0 ) {
            std::this_thread::yield();
            continue;
        }

        _jobs->shutdown();

        _jobs->run( [ & ]() { _q->_s->on_instant( q ); } );

        for ( auto& as : _aspects ) {
            _jobs->run( [ & ]() { as->update( q ); } );
        }

        while ( _jobs->busy() ) {
            std::this_thread::yield();
        }

        _q->_s->on_invoke( 0 );
    }
}

Data::~Data() {
    delete _jobs;
    for ( auto as : _aspects ) {
        delete as;
    }
}

CUB_NS_END
