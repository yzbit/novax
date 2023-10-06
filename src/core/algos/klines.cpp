#include "klines.h"

CUB_NS_BEGIN

Kline::Kline( const code_t& code_, const period_t& p_ )
    : _symbol( code_ )
    , _period( p_ ) {
}

Kline* Kline::create( const arg_pack_t& arg_ ) {
    return new Kline( ( string_t )arg_[ 0 ], ( const period_t )arg_[ 1 ] );
}

void Kline::on_init() {
    add_series( 1, 40 );
}

void Kline::on_calc( const quotation_t& q_ ) {
    // 合并生成指定的K线
}

CUB_NS_END
