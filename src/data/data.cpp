#include "data.h"

#include "ctp_proxy.h"
#include "market.h"

CUB_NS_BEGIN

Data& Data::instance() {
    static Data data;

    return data;
}

// 来的数据可能是秒级，毫秒级，分钟级，甚至按照天来的，我们必须适配
// aspect应该是数据的接口,客户调用data.subscribe的时候，自动创建aspect，如果两个aspect有关联，比如1分钟和10分钟明显前者可以拼成后者，虽然没有太大必要，
// 直接通过ticp拼是没什么区别的（性能上无差）
void Data::on_data( const quotation_t& tick_ ) {
    _job.clear();

    _job.run( STATEGY.fire( currnet_bar ), true );  // 单开一个线程

    for ( auto& as : aspects ) {
        _job.run( as->update() );
    }

    _job.join();

    STRATEGY.invoke();
}

void Data::on_msg( const msg::header_t& h_ ) {
    switch ( h_.id ) {
    case msg::mid_t::book_data:
        break;

    default:
        break;
    }
}

Data::Data() {
    REACTOR.sub( { msg::mid_t::svc_data }, [ & ]( const msg::header_t& h ) {
        if ( h.id != msg::mid_t::svc_data ) {
            LOG_TRACE( "receive bad msg %u", h.id );
            return;
        }

        on_data( { 0 } );
    } );

    REACTOR.sub( {}, [ & ]( const msg::header_t& h ) {
        on_msg( h );
    } );

    _market = Market::create();

    if ( !_market || _market->init() != 0 ) {
        LOG_INFO( "create market failed" );
        delete _market;
        _market = nullptr;
    }
}

Data::~Data() {
}

int Data::subscribe( const code_t& code_ ) {
    return _market->subscribue( code_ );
}

int Data::unsubscribe( const code_t& code_ ) {
    return _market->unsubscribue( code_ );
}

CUB_NS_END
