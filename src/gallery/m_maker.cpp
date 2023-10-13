#include "m_maker.h"

void MarketMaker::on_init( Context& c ) {
}

void MarketMaker::on_refresh( Context& c ) {
    price_t spread  = c.q.ask - c.q.bid;
    price_t percent = spread / c.q.ask * 100;
    price_t mprice  = ( c.q.ask + c.q.bid ) / 2;

    if ( percent > 0.1 && c.position() == 0 ) {
        buy_price  = c.q.bid + spread * 0.05;
        sell_price = c.q.ask - spread * 0.05;

        //下多条腿,还是分别下

        c.buylong( "btc/usdt", 0.001, buy_price, otype_t::fok );
        c.sellshort( "btc/usdt", 0.001, sell_price, otype_t::fok );
    }

    c.close();
}
