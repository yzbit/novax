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
    _data->init( []( Series::element_t& e_ ) {
        e_.p = new candle_t();
    } );
}

// todo 所有的指标的shift不应该由指标来调,因为都是和aspect(kline)同步的,可以先不做,避免过度优化
// 分钟k一定是和分钟对齐,至少结束的时候是和分钟对齐
// 如果q-start大于某个周期则一定要换下一个K
//---把q-start换成秒,如果大于1秒,则一定是下一个,如果大于60s则一定是下一分钟;同理,如果大于1小时一定是另外一个小时k,不过对于小时k来说,然后应该用时钟
// 来定义,也就是当时钟过了5点,那么一定是第6根K线,理论不应该出现第5小时的tick时间是5.00.001
/*
基于第一部分已经可以完成将收到的行情数据写入csv，但仔细观察csv就会发现有的时候会收到一些不正常数据，比如下面这条夜盘开盘前登陆可能收到的数据：
TradingDay,InstrumentID,...,UpdateTime,UpdateMillisec...
20201126,au2012,...,18:58:17,200,...
这一条数据更新时间戳是非交易时间，而且毫秒时间戳也不正常。一般在开盘前提前登陆都会收到这样的垃圾数据，是因为CTP初始化时会先导入一条行情数据。
有时日盘连上会重新再收到一次夜盘的行情数据，是因为CTP日盘初始化时会将夜盘的数据重新跑一遍，所以还会再推一遍夜盘行情数据。
这些都是垃圾数据，需要清洗掉。可以简单地采用行情里面的时间戳与本地时间戳比大小的方式清洗，不过要事先保证本地时间戳正确，如下
*/
void Kline::on_calc( const quotation_t& q_ ) {
    auto& s = recent();

    bool next_bar = true;

    if ( next_bar ) {
        shift();
    }
}

CUB_NS_END
