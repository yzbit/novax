#include "../core/dci_role.h"
#include "../core/definitions.h"
#include "../core/log.hpp"
#include "../core/models.h"
#include "../core/order_mgmt.h"
#include "../core/proxy.h"

namespace cub {
struct XContext : cub::MgmtContext {
    int put_order( const order_t& o_ ) {
        return 0;
    }

    int del_order( oid_t id_ ) {
        return 0;
    }
};
}  // namespace cub

int main() {
    LOG_ENABLE_STDOUT();

    auto om = new cub::OrderMgmt( 0 );

    om->start();
    om->sellshort( "rb2410", 1, 6543.0, cub::otype_t::market );
    om->buylong( "rb2410", 1, 6543.0, cub::otype_t::market );
    om->position( "rb2410" );

    return 0;
}