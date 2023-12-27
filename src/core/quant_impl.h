#ifndef D392EBBF_A7F9_4ED1_B8F6_BE4B2E02DA07
#define D392EBBF_A7F9_4ED1_B8F6_BE4B2E02DA07

#include "context.h"
#include "dci_role.h"
#include "ns.h"
#include "quant.h"
#include "timer.h"

SATURN_NS_BEGIN
struct OrderMgmt;
struct Data;
struct Strategy;

struct QuantImpl : Quant, DataContext, Context {
    QuantImpl();

    // protected:
    //     int put_order( const order_t& o_ ) override;
    //    int del_order( oid_t id_ ) override;

protected:
    void update( const quotation_t& q_ ) override;
    void invoke() override;

protected:
    int execute( Strategy* s_ ) override;

protected:
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
    int  init();
    void quote( const quotation_t& q_ );
    void update( const order_t& o_ );

private:
    void ontick();

private:
    Data*      _d = nullptr;
    Strategy*  _s = nullptr;
    Context*   _c = nullptr;
    OrderMgmt* _o = nullptr;

private:
    bool  _running = true;
    bool  _working = false;
    Timer _timer;
};

SATURN_NS_END

#endif /* D392EBBF_A7F9_4ED1_B8F6_BE4B2E02DA07 */
