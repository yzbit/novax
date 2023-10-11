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

// facade 类
struct Context {
    //---load trade strategy
    int load( const string_t& strategy_name_, const arg_pack_t& arg_ );
    int load( std::unique_ptr<Strategy> a_ );

    //----apects---
    Aspect* aspect( const code_t& symbol_, const period_t& period_, int count_ );
    Aspect* aspect();

    //------市价下单------
    int pshort( const code_t& c_, vol_t qty_, price_t price_ = 0, otype_t mode_ = otype_t::market );
    int plong( const code_t& c_, vol_t qty_, price_t price_ = 0, otype_t mode_ = otype_t::market );
    int cshort( const code_t& c_, vol_t qty_ = 0, price_t price_ = 0, otype_t mode_ = otype_t::market );
    int clong( const code_t& c_, vol_t qty_ = 0, price_t price_ = 0, otype_t mode_ = otype_t::market );
    int close( const code_t& c_, vol_t qty_ = 0, price_t price_ = 0, otype_t mode_ = otype_t::market );

    //-------仓位查询-------
    vol_t position();  // 已成交持仓
    vol_t position( const code_t& c_ );
    vol_t pending();  // 未成交持仓
    vol_t pending( const code_t& c_ );

    //--------账户查询------
    quotation_t q;
    fund_t      f;

    //--------K线相关------
    Kline&    kline();
    candle_t& bar( int index_ = 0 );

    //--------交易相关-------
    price_t put_price();
    price_t last_deal();
    int     last_entry();  // 最近入场的k线
    int     last_exit();   // 最近出场的k线//和aspect相关
};

CUB_NS_END

#endif /* C2E26F98_58D2_4FB6_9B05_CB4ED59A65C3 */
