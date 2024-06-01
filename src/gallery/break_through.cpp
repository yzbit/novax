/************************************************************************************
The MIT License

Copyright (c) 2024 YaoZinan  [zinan@outlook.com, nvx-quant.com]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

* \author: yaozn(zinan@outlook.com)
* \date: 2024
**********************************************************************************/

#include "break_through.h"

#include "../core/log.hpp"
#include "context.h"

void BreakTh::on_init( Context* c_ ) {
    if ( !c_->load( _code, { period::type_t::hour, 2 }, 30 ) ) {
        LOG_INFO( "load code failed: code=%s,period={%d %d}, count=%d", _code, ( int )period::type_t::hour, 2, 30 );
        return;
    }
}

void BreakTh::on_ck( const tick& ) {
}

#define DELAY_TRADING ( 20 * 60 )  // 20MIN

// 不在刚开盘的时候交易
void BreakTh::on_invoke( Context* c_ ) {
    auto t = c_->clock.time_since_open();

    if ( t < DELAY_TRADING && 0 == c_->position() ) return;

    // 还是按照基本形态来交易,先统计一下平均止损的大小




    if ( c.position() == 0 ) {
        if ( c.bar().close() < c.kline().llv( 10, pricetype_t::close ) && t > 15 * 60 ) {  // 开盘15分钟后
            oid id = c.pshort( _code, 1, c.bar().close, ord_type::market );

            if ( id == 0 ) {
                LOG_INFO( "sell short failed" );
                return;
            }
        }
        else if ( c.bar().close() > c.kline().hhv( 10, pricetype_t::close ) && t > 15 * 60 ) {
            oid id = c.plong( _code, 1, c.bar().close, ord_type::market );

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