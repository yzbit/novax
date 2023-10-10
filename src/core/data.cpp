#include "data.h"

#include "aspect.h"
#include "log.hpp"
#include "market.h"
#include "reactor.h"
#include "strategy.h"

CUB_NS_BEGIN

Data& Data::instance() {
    static Data data;

    return data;
}

// 来的数据可能是秒级，毫秒级，分钟级，甚至按照天来的，我们必须适配
// aspect应该是数据的接口,客户调用data.subscribe的时候，自动创建aspect，如果两个aspect有关联，比如1分钟和10分钟明显前者可以拼成后者，虽然没有太大必要，
// 直接通过tick拼是没什么区别的（性能上无差）
void Data::on_data( const quotation_t& tick_ ) {
    _jobs->shutdown();

    _jobs->run( [ & ]() { STRATEGY.fire( &tick_ ); } );  // 单开一个线程

    for ( auto& as : _aspects ) {
        _jobs->run( [ & ]() { as->update( &tick_ ); } );
    }

    while ( _jobs->busy() ) {
        std::this_thread::yield();
    }

    STRATEGY.invoke( 0 );
}

Data::Data() {
    _jobs = TaskQueue::create( 4 );
}

Data::~Data() {
}

CUB_NS_END
