#include "../core/dci_role.h"
#include "../core/definitions.h"
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
    auto om = new cub::OrderMgmt( new cub::XContext() );

     om->start();
    // om->sellshort();
    // om->buylong();

    // om->position();

    // om->start();

    return 0;
}