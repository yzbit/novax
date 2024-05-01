#ifndef C2E26F98_58D2_4FB6_9B05_CB4ED59A65C3
#define C2E26F98_58D2_4FB6_9B05_CB4ED59A65C3
#include <atomic>
#include <memory>

#include "clock.h"
#include "definitions.h"
#include "kline.h"
#include "models.h"
#include "ns.h"

NVX_NS_BEGIN
struct Aspect;
struct Quant;
struct Context {
    const quotation_t& qut() const;
    const fund_t       fund() const;

    Clock&  clock();
    Aspect* load( const code_t& symbol_, const period_t& period_, int count_ );
    int     open( const code_t& c_, vol_t qty_, price_t sl_ = 0, price_t tp_ = 0, price_t price_ = 0, otype_t mode_ = otype_t::market );
    int     close( const code_t& c_, vol_t qty_, price_t price_ = 0, otype_t mode_ = otype_t::market );
    vol_t   position() const;
    vol_t   position( const code_t& c_ ) const;
    vol_t   pending() const;
    vol_t   pending( const code_t& c_ ) const;
    price_t put_price() const;
    price_t last_deal() const;
    kidx_t  last_entry() const;
    kidx_t  last_exit() const;

    Context( Quant* );
    void update_qut( const quotation_t& q_ );
    void update_fund( const fund_t& f_ );

private:
    Quant*              _q;
    quotation_t         _qut;
    std::atomic<fund_t> _fund;
};

NVX_NS_END

#endif /* C2E26F98_58D2_4FB6_9B05_CB4ED59A65C3 */
