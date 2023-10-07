#include "klines.h"

#define BAR_TRACK 0
CUB_NS_BEGIN

Kline::Kline( const code_t& code_, const period_t& p_, int series_count_ )
    : _count( series_count_ )
    , _symbol( code_ )
    , _period( p_ )
    , _curr_bar( 0 ) {
}

Kline* Kline::create( const arg_pack_t& arg_ ) {
    auto c = ( const char* )arg_[ 0 ];

    return new Kline( ( string_t )arg_[ 0 ], ( const period_t )arg_[ 1 ], ( int )arg_[ 2 ] );
}

void Kline::on_init() {
    _data = add_series( BAR_TRACK, _count, ::free );
}

void Kline::on_calc( const quotation_t& q_ ) {
    // 合并生成指定的K线
    auto s = track();

    s->append( new int );

    auto bar = s->at( 0 ).p;
}

CUB_NS_END
