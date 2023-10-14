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

// 接口的设计：实际交易的过程中，按照订单平仓的可能性其实蛮小的，应该还是按照合约名称+仓位 平仓的可能性更大
// 高频交易可能下单，撤单，平仓快速发生，此时oid显然是用的
struct MgmtContext;
struct OrderMgmt {
    struct Delegator {
        virtual ~Delegator() {}
        virtual int start()                  = 0;
        virtual int stop()                   = 0;
        virtual int put( const order_t& o_ ) = 0;
        virtual int cancel( oid_t o_ )       = 0;
    };

    OrderMgmt( MgmtContext* c_ );

    oid_t sellshort( const oattr_t& attr_, price_t sl_ = 0, price_t tp_ = 0, const text_t& remark = "open short" );
    oid_t buylong( const oattr_t& attr_, price_t sl_ = 0, price_t tp_ = 0, const text_t& remark_ = "open buy" );

    int sell( const oattr_t& a_ = { "", 0, 0, otype_t::market } );
    int buy( const oattr_t& a_ = { "", 0, 0, otype_t::market } );
    int cancel( oid_t id_ );
    int close( oid_t id_ );

    void update( oid_t id_, ostatus_t status_ );
    void update( const order_t& o_ );

    // >0 表示long多余short
    vol_t position( const code_t& code_ );
    vol_t short_position( const code_t& code_ );
    vol_t long_position( const code_t& code_ );

private:
    void  herge( order_t* src_, const order_t* update_ );
    void  accum( order_t* src_, const order_t* update_ );
    int   close( odir_t dir_, const oattr_t& a_ = { "", 0, 0, otype_t::market } );
    oid_t put( const oattr_t& attr_ );

private:
    oid_t       oid();
    order_t*    get( oid_t id_ );
    position_t* position( const code_t& code_, bool long_ );
    void        create_position( const code_t& code_ );

private:
    using instrument_p_t = std::array<position_t, 2>;                                //! 仓位
    using OrderDetails   = std::unordered_map<oid_t, order_t*>;                      //! 所有的订单列表
    using InsPosition    = std::unordered_map<code_t, instrument_p_t, code_hash_t>;  //! 每个合约有正反两个方向的持仓

    OrderDetails _book;
    InsPosition  _ins_position;
    std::mutex   _mutex;

private:
    static std::atomic<oid_t> _init_id;

private:
    MgmtContext* _c;
    Delegator*   _d;
};

CUB_NS_END

#endif /* A0120CD0_E1AB_40A0_93BC_9BE6188CDA2A */
