#include "order_mgmt.h"

#include "reactor.h"

CUB_NS_BEGIN

OrderMgmt::OrderMgmt() {
    REACTOR.sub( { msg::mid_t::svc_order }, [ & ]( auto& h ) {
        on_update( order_t() );
    } );
}

void OrderMgmt::on_update( const order_t& o_ ) {
}

CUB_NS_END