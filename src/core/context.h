#ifndef C2E26F98_58D2_4FB6_9B05_CB4ED59A65C3
#define C2E26F98_58D2_4FB6_9B05_CB4ED59A65C3
#include <memory>

#include "clock.h"
#include "definitions.h"
#include "kline.h"
#include "models.h"
#include "ns.h"
#include "strategy.h"

NVX_NS_BEGIN
struct Indicator;
struct Aspect;

// facade 类
struct Context {
    static Context* create();

    quotation_t q;
    fund_t      f;
    Clock       clock;

    //--技术指标不应该在这里出现
    //----apects---
    // Aspect* asp( int id_ );
    // 创建完了自然要保存下来,否则每次还去查找实在没必要
    // 但是我们可以有默认的asp
    Aspect* load( const code_t& symbol_, const period_t& period_, int count_ );

    //--qty_ < 0表示卖空，>0表示做多
    int open( const code_t& c_, vol_t qty_, price_t sl_ = 0, price_t tp_ = 0, price_t price_ = 0, otype_t mode_ = otype_t::market );

    //--qty_ < 0表示平空，>0表示平多,0平所有
    int close( const code_t& c_, vol_t qty_, price_t price_ = 0, otype_t mode_ = otype_t::market );

    //-------仓位查询-------
    vol_t position();  // 已成交持仓
    vol_t position( const code_t& c_ );
    vol_t pending();  // 未成交持仓
    vol_t pending( const code_t& c_ );

    //--------交易相关-------
    price_t put_price();
    price_t last_deal();
    int     last_entry();  // 最近入场的k线
    int     last_exit();   // 最近出场的k线//和aspect相关

protected:
    Context();
};

NVX_NS_END

#endif /* C2E26F98_58D2_4FB6_9B05_CB4ED59A65C3 */
