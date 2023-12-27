#include "break_through.h"

#include "../core/log.hpp"
#include "context.h"

void BreakTh::on_init( Context* c_ ) {
    if ( !c_->load( _code, { period_t::type_t::hour, 2 }, 30 ) ) {
        LOG_INFO( "load code failed: code=%s,period={%d %d}, count=%d", _code, ( int )period_t::type_t::hour, 2, 30 );
        return;
    }
}

void BreakTh::on_ck( const quotation_t& ) {
}

#define DELAY_TRADING ( 20 * 60 )  // 20MIN

// 不在刚开盘的时候交易
void BreakTh::on_invoke( Context* c_ ) {
    auto t = c_->clock.time_since_open();

    if ( t < DELAY_TRADING && 0 == c_->position() ) return;

    // 还是按照基本形态来交易,先统计一下平均止损的大小




    if ( c.position() == 0 ) {
        if ( c.bar().close() < c.kline().llv( 10, pricetype_t::close ) && t > 15 * 60 ) {  // 开盘15分钟后
            oid_t id = c.pshort( _code, 1, c.bar().close, otype_t::market );

            if ( id == 0 ) {
                LOG_INFO( "sell short failed" );
                return;
            }
        }
        else if ( c.bar().close() > c.kline().hhv( 10, pricetype_t::close ) && t > 15 * 60 ) {
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