#include "strategy.h"

CUB_NS_BEGIN

struct XAlgo::Strategy {
    void on_refresh( Context& c ) {
        vol_t p = c.position();

        if ( p != 0 ) {
            sltp();
        }

        if ( c.has_pending() ) {
            c.cancel();
        }

        if ( ma1[ 0 ] < ma2[ 0 ] && c.price() < c.hhv( 5 ) ) {
            vol_t qty = c.margin * 0.5 / c.q.price * 10;
            oid_t id  = c.sellshort( c.q.price, qty, mode_t::market );
            if ( id < 0 ) {
                fprintf( stderr, "putorder fail" );
            }
        }

        // 获取当前K线的状态
        if ( asp_rb->[ 0 ] < asp_rb -> [ 1 ] ) {
        }
    }

    //--成交回报--方便进行快速处理
    void on_trade( Context& c ) {
    }

    void on_error( Context& c ) {
    }

    void on_init( Context& c ) {
        asp_rb = c.create_aspect( "rb2410", period_t{ period_t::type_t::hour, 2 } );
        ma1    = asp_rb->attach( "ma", { 1, 2, 3, 4, 5 }, "ma1234" );
        ma2    = asp_rb->attach( "ma", { 1, 2, 3, 4, 5 }, "ma1234" );
    }

    void on_instant( const quotation_t& q_ ) {
        if ( q_.price < open_price ) {
            c.close();
        }
    }

private:
    price_t    open_price;
    id_t       asp_rb;
    Indicator* ma1;
    Indicator* ma2;
};

CUB_NS_END