#ifndef C2E26F98_58D2_4FB6_9B05_CB4ED59A65C3
#define C2E26F98_58D2_4FB6_9B05_CB4ED59A65C3
#include "definitions.h"
#include "models.h"
#include "ns.h"

CUB_NS_BEGIN
struct Indicator;
struct Algo;
struct Aspect;

// facade 类
struct Context {
    Aspect* create_aspect( const code_t& symbol_, const period_t& period_, int count_, const string_t& alias_ );
    int     load( const string_t& algo_name_, const ArgPack& arg_ );
    int     load( Algo* a_ );

    //------市价下单------
    int pshort( const code_t& c_, vol_t qty_ );
    int plong( const code_t& c_, vol_t qty_ );
    int cshort( const code_t& c_, vol_t qty_ = 0 );
    int clong( const code_t& c_, vol_t qty_ = 0 );

    //------高级下单模式-----
    int pshort( const attr_t& a_ );
    int plong( const attr_t& a_ );
    int cshort( const attr_t& a_ );
    int clong( const attr_t& a_ );

    //-------仓位查询-------
    Portfolio    p();
    quotation_t& q();
    vol_t        position();  //已成交持仓
    vol_t        position( const code_t& c_ );
    vol_t        pending();  //未成交持仓
    vol_t        pending( const code_t& c_ );

    //--------账户查询------
    fund_t f();

    //--------K线相关------
    price_t  hhv( int bars_ );
    price_t  llv( int bars_ );
    candle_t bar( int index_ );
    price_t  put_price();
    price_t  last_deal();
    int      last_entry();  //最近入场的k线
    int      last_exit();   //最近出场的k线//和aspect相关
};

CUB_NS_END

#endif /* C2E26F98_58D2_4FB6_9B05_CB4ED59A65C3 */
