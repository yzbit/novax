#ifndef A0120CD0_E1AB_40A0_93BC_9BE6188CDA2A
#define A0120CD0_E1AB_40A0_93BC_9BE6188CDA2A

#include <atomic>
#include <mutex>
#include <optional>
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
        virtual int start()                     = 0;
        virtual int stop()                      = 0;
        virtual int put( const order_t& o_ )    = 0;
        virtual int cancel( const order_t& o_ ) = 0;
    };

    ~OrderMgmt();
    OrderMgmt( MgmtContext* c_ );

    void start();
    void stop();

    oid_t sellshort( const code_t& code_,
                     const vol_t   qty_,
                     const price_t price_ = 0,
                     const otype_t mode_  = otype_t::market,
                     const price_t sl_    = 0,
                     const price_t tp_    = 0,
                     const text_t& remark = "open short" );

    oid_t buylong( const code_t& code_,
                   const vol_t   qty_,
                   const price_t price_ = 0,
                   const otype_t mode_  = otype_t::market,
                   const price_t sl_    = 0,
                   const price_t tp_    = 0,
                   const text_t& remark = "open buy" );

    int sell( const code_t& code_,
              const vol_t   qty_   = 0,
              const price_t price_ = 0,
              const otype_t mode_  = otype_t::market,
              const text_t& remark = "close buy" );

    int buy( const code_t& code_,
             const vol_t   qty_   = 0,
             const price_t price_ = 0,
             const otype_t mode_  = otype_t::market,
             const text_t& remark = "close short" );

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
    int   close( const order_t& r_ );
    oid_t put( const odir_t& dir_,
               const code_t& code_,
               const vol_t   qty_,
               const price_t price_,
               const otype_t mode_,
               const price_t sl_,
               const price_t tp_,
               const text_t& remark_ );

private:
    oid_t       oid();
    order_t*    get( oid_t id_ );
    position_t* position( const code_t& code_, bool long_ );

private:
    using portfilio_t  = std::array<position_t, 2>;                             //! 仓位, [0]-long, [1]-short
    using OrderDetails = std::unordered_map<oid_t, order_t*>;                   //! 所有的订单列表
    using InsPosition  = std::unordered_map<code_t, portfilio_t, code_hash_t>;  //! 每个合约有正反两个方向的持仓

    OrderDetails _book;
    InsPosition  _ins_position;
    std::mutex   _mutex;

private:
    static std::atomic<oid_t> _init_id;

private:
    MgmtContext* _c = nullptr;
    Delegator*   _d = nullptr;
};

CUB_NS_END

#endif /* A0120CD0_E1AB_40A0_93BC_9BE6188CDA2A */
