#ifndef BE5CA6EA_80F4_4820_A89B_42E4FA6B819C
#define BE5CA6EA_80F4_4820_A89B_42E4FA6B819C

#include "context.h"

CUB_NS_BEGIN
struct QuantImpl;
struct Aspect;
struct ContextImpl : Context {
    ContextImpl( QuantImpl* q );

    Aspect* aspect() override;
    Aspect* add_aspect( const code_t& symbol_, const period_t& period_, int count_ ) override;
    int     pshort( const code_t& c_, vol_t qty_, price_t price_ = 0, otype_t mode_ = otype_t::market ) override;
    int     plong( const code_t& c_, vol_t qty_, price_t price_ = 0, otype_t mode_ = otype_t::market ) override;
    int     cshort( const code_t& c_, vol_t qty_ = 0, price_t price_ = 0, otype_t mode_ = otype_t::market ) override;
    int     clong( const code_t& c_, vol_t qty_ = 0, price_t price_ = 0, otype_t mode_ = otype_t::market ) override;

    vol_t position() override;  // 已成交持仓
    vol_t position( const code_t& c_ ) override;
    vol_t pending() override;  // 未成交持仓
    vol_t pending( const code_t& c_ ) override;

    //--------K线相关------
    Kline&    kline() override;
    candle_t& bar( int index_ = 0 ) override;

private:
    Aspect*    _def_asp;
    QuantImpl* _q;
};

CUB_NS_END

#endif /* BE5CA6EA_80F4_4820_A89B_42E4FA6B819C */
