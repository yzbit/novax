#ifndef C2E26F98_58D2_4FB6_9B05_CB4ED59A65C3
#define C2E26F98_58D2_4FB6_9B05_CB4ED59A65C3
#include <memory>

#include "definitions.h"
#include "models.h"
#include "ns.h"
#include "strategy.h"

CUB_NS_BEGIN
struct Indicator;
struct Aspect;
struct Clock;
struct QuantImpl;
// facade 类
struct Context {
    static Context* create( QuantImpl* q );

    quotation_t q;
    fund_t      f;
    Clock*      clock;

    //----apects---
    virtual Aspect* add_aspect( const code_t& symbol_, const period_t& period_, int count_ ) = 0;
    virtual Aspect* aspect()                                                                 = 0;

    //-----下单------
    virtual int pshort( const code_t& c_, vol_t qty_, price_t price_ = 0, otype_t mode_ = otype_t::market )     = 0;
    virtual int plong( const code_t& c_, vol_t qty_, price_t price_ = 0, otype_t mode_ = otype_t::market )      = 0;
    virtual int cshort( const code_t& c_, vol_t qty_ = 0, price_t price_ = 0, otype_t mode_ = otype_t::market ) = 0;
    virtual int clong( const code_t& c_, vol_t qty_ = 0, price_t price_ = 0, otype_t mode_ = otype_t::market )  = 0;

    //-------仓位查询-------
    virtual vol_t position()                   = 0;  // 已成交持仓
    virtual vol_t position( const code_t& c_ ) = 0;
    virtual vol_t pending()                    = 0;  // 未成交持仓
    virtual vol_t pending( const code_t& c_ )  = 0;

    //--------K线相关------
    virtual Kline&    kline()               = 0;
    virtual candle_t& bar( int index_ = 0 ) = 0;

    //--------交易相关-------
    price_t put_price();
    price_t last_deal();
    int     last_entry();  // 最近入场的k线
    int     last_exit();   // 最近出场的k线//和aspect相关

protected:
    Context() {}
};

CUB_NS_END

#endif /* C2E26F98_58D2_4FB6_9B05_CB4ED59A65C3 */
