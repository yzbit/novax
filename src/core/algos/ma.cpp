#include <stdio.h>

#include "ma.h"

SATURN_NS_BEGIN

Ma* Ma::create( const arg_pack_t& p_ ) {
    return new Ma( p_[ 0 ], ( int )p_[ 1 ] );
}

Ma::Ma( const std::string& code_, int period_ ) {
    fprintf( stderr, "%s %d\n", code_.c_str(), period_ );
    add_series( 1, period_ );
}

void Ma::on_calc( const Kline& ref_, const quotation_t& q_ ) {
    if ( ref_.bar_cnt() < period_ ) {
        return 0;
    }

    double accum = 0;

    // 如果bar移动位置了,则移动平均也要移动,否则就原地计算
    accum += q_.last;

    //从当前的bar计算到ref_bar.cnt
    for ( int start = this->bar_cnt(); start < ref_.bar_cnt(); ++start ) {

    }
    //
}

SATURN_NS_END