#include "break_through.h"

void BreakTh::on_init( Context* c ) {
    if ( c.aspect()->load( _code, { period_t::type_t::hour, 2 }, 30 ) > 0 ) {
        LOG_INFO( "load code failed: code=%s,period={%d %d}, count=%d", _code, ( int )period_t::type_t::hour, 2, 30 );
        return;
    }

    _ma = c.aspect()->attach( "Ma", { 20, Ma::mid } );
    if ( !_ma ) {
        LOG_INFO( "create ma indicator failed" );
    }
}

void BraekTh::on_instant( const quotation_t& ) {
}

//不在刚开盘的时候交易
void BreakTh::on_invoke( Context* c ) {
    auto t = c.time_since_open();
    auto t = c.time_since( open_time() );

    if ( c.position() == 0 ) {
        if ( c.bar().close() < c.kline().llv( 10, Kline::pricetype_t::close ) && t > 15 * 60 ) {  //开盘15分钟后
            oid_t id = c.pshort( _code, 1, c.bar().close, otype_t::market );

            if ( id == 0 ) {
                LOG_INFO( "sell short failed" );
                return;
            }
        }
        else if ( c.bar().close() > c.kline().hhv( 10, Kline::pricetype_t::close ) && t > 15 * 60 ) {
            oid_t id = c.plong( _code, 1, c.bar().close, otype_t::market );

            if ( id == 0 ) {
                LOG_INFO( "sell short failed" );
                return;
            }
        }
    }
    else if ( c.position() < 0 ) {
        if ( c.bar( 1 ).is_red() && c.bar( 1 ).body_up() > _ma->at( 1 ) ) {
            c.close( _code );
        }
    }
    else {
        if ( c.bar( 1 ).is_black() && c.bar( 1 ).body_low() < _ma->at( 1 ) ) {
            c.close( _code );
        }
    }
}