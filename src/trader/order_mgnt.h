#ifndef A0120CD0_E1AB_40A0_93BC_9BE6188CDA2A
#define A0120CD0_E1AB_40A0_93BC_9BE6188CDA2A

#include "definitions.h"
#include "ns.h"

CUB_NS_BEGIN
struct OrderMgmt {
    oid_t buy( amnt_t        lot_,
               price_t       price_,
               int           mode_,
               price_t       sl_,
               price_t       tp_,
               const code_t& code_,
               const text_t& remark_ = "open buy" );

    oid_t sellshort( amnt_t        lot_,
                     price_t       price_,
                     int           mode_,
                     price_t       sl_,
                     price_t       tp_,
                     const code_t& code_,
                     const text_t& remark = "open short" );

    int sell( amnt_t        lot_,
              price_t       price_,
              const code_t& code_,
              const text_t& remark = "close long" );

    int cover( amnt_t        lot_,
               price_t       price_,
               const code_t& code_,
               const text_t& remark = "close short" );
};

CUB_NS_END

#endif /* A0120CD0_E1AB_40A0_93BC_9BE6188CDA2A */
