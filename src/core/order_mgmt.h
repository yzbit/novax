#ifndef A0120CD0_E1AB_40A0_93BC_9BE6188CDA2A
#define A0120CD0_E1AB_40A0_93BC_9BE6188CDA2A

#include <atomic>
#include <mutex>
#include <unordered_map>

#include "definitions.h"
#include "models.h"
#include "ns.h"

#define LONG_POSITION 0
#define SHORT_POSITION 1

CUB_NS_BEGIN

struct Trader;

// 接口的设计：实际交易的过程中，按照订单平仓的可能性其实蛮小的，应该还是按照合约名称+仓位 平仓的可能性更大
// 高频交易可能下单，撤单，平仓快速发生，此时oid显然是用的
struct OrderMgmt {
    static OrderMgmt& instance();
    OrderMgmt();

    oid_t sellshort( const oattr_t& attr_, price_t sl_, price_t tp_, const text_t& remark = "open short" );
    oid_t buylong( const oattr_t& attr_, price_t sl_, price_t tp_, const text_t& remark_ = "open buy" );

    int close( const oattr_t& a_ = { "", 0, 0, otype_t::market } );
    int cancel( oid_t id_ );
    int close( oid_t id_ );

    void update( oid_t id_, ostatus_t status_ );
    void update( const order_t& o_ );

    // >0 表示long多余short
    vol_t position( const code_t& code_ );
    vol_t short_position( const code_t& code_ );
    vol_t long_position( const code_t& code_ );

private:
    void herge( order_t* src_, const order_t* update_ );
    void accum( order_t* src_, const order_t* update_ );

private:
    oid_t       oid();
    order_t*    get( oid_t id_ );
    position_t* position( const code_t& code_, bool long_, bool new_ );

private:
    using instrument_p_t = std::array<position_t, 2>;
    using OrderDetails   = std::unordered_map<oid_t, order_t*>;
    using InsPosition    = std::unordered_map<code_t, instrument_p_t, code_hash_t>;

    std::atomic<oid_t> _init_id = 0;
    OrderDetails       _book;
    InsPosition        _ins_position;
    Trader*            _trader;
    std::mutex         _mutex;
};

CUB_NS_END

#define OMGMT cub::OrderMgmt::instance()

#endif /* A0120CD0_E1AB_40A0_93BC_9BE6188CDA2A */
