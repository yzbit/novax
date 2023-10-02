#ifndef A0120CD0_E1AB_40A0_93BC_9BE6188CDA2A
#define A0120CD0_E1AB_40A0_93BC_9BE6188CDA2A

#include "definitions.h"
#include "models.h"
#include "ns.h"

CUB_NS_BEGIN
struct OrderMgmt {
    static OrderMgmt& instance();
    OrderMgmt();

    oid_t sellshort( const oattr_t& attr_, price_t sl_, price_t tp_, const text_t& remark = "open short" );
    int   cover( oid_t id );
    int   cover( const oattr_t& attr_, const text_t& remark = "close short" );
    oid_t buylong( const oattr_t& attr_, price_t sl_, price_t tp_, const text_t& remark_ = "open buy" );
    int   sell( const oattr_t& attr_, const text_t& remark = "close long" );

private:
    void on_update( const order_t& o_ );
};

CUB_NS_END

#define OMGMT cub::OrderMgmt::instance()

#endif /* A0120CD0_E1AB_40A0_93BC_9BE6188CDA2A */
