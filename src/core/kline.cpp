#include "kline.h"

#include "clock.h"
#include "log.hpp"

#define BAR_TRACK 0
NVX_NS_BEGIN

Kline::Kline( const code_t& code_, const period_t& p_, int series_count_ )
    : _count( series_count_ )
    , _symbol( code_ )
    , _period( p_ )
    , _curr_bar( 0 ) {
    init();
}

void Kline::init() {
    _bars = new BarSeries( _count );
}

candle_t& Kline::bar( int index_ ) {
    return _bars->get( index_ );
}

quotation_t& Kline::qut() {
    return _qut;
}

//--period如果是天，当然是以每天的开盘价作为上一个x线的结束
//--以小时为单位，则必然是以开盘+n小时作为结果，而且以天为单位
//--暂且不处理超过日的周期
bool Kline::is_new_bar( const quotation_t& q_ ) {
#if 0  // todo
    uint32_t p     = _period;
    uint32_t shift = CLOCK_OF( q_.ex ).open_shift();

    return ( q_.time.to_unix_time() - shift ) / p != ( bar().time.to_unix_time() - shift ) / p;
#endif
    return false;
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
/*我们只要和开盘时间对齐即可

比如开盘是9:00:00.000 那么当我们的软件启动的时候如果来了一个k线的时间是  10:31:00.000 ，如果我们按照5分钟收集
从9+n*5 - 10：31
实际上从31到34都应该是属于10：30的k线序列，而不是10：31-10：36属于第一根K线
按照秒来收集足够了，假设1秒为单位，那么每一秒是一根k线

9:00:00.456--这是第一根k线

9:00:01.123--这是第二根
9:00:01.786--
9:00:02.111--这是第三根

把周期全部转成s，
1- 按照开盘时间对齐： 从开盘到现在必须是n秒的整数倍
2- 小于ns的都算n根线
3- 按照实际时间来算， 假如以3分钟或者3小时来统计，那么必然的，一根k线会跨越两天，但是这不重要

文华财经的时间分布都以交易日来做的，如果一天只有7个小时，那么你按照5小时为周期，就会导致5+2，每天分为2个k线，第二个k线只有2个小时
我们可以提供两种模式，一种按照自然日来分割，日内按照K线周期来分，最后不足一个周期就算了; 第二按照每根k线足够的周期来生成
考虑到开盘价很重要，这么做可能意义不是很大
*/
void Kline::calc( const quotation_t& q_, int total_ ) {
    _qut = q_;

    // auto r = recent();

    // 此时可以认为是新开盘
    if ( abs( CLOCK.now( q_.ex ) - q_.time.to_unix_time() ) > 3 * 60 ) {
        LOG_INFO( "market open ;obsolete data recieved: %u %u", CLOCK.now( q_.ex ), q_.time.to_unix_time() );
        return;
    }

    candle_t* k = nullptr;

    if ( is_new_bar( q_ ) ) {
        // 结束上一个，并且开始下一个，但是对于新开始的来说，上个并不存在
        k = &bar();
        if ( k->time.is_valid() ) {
            k->time   = q_.time;
            k->volume = q_.volume - k->volume;

            _bars->shift();
            k = &bar();
            memset( k, 0, sizeof( candle_t ) );
        }

        k->time   = q_.time;  // 开始时间
        k->volume = q_.volume;
        k->open   = q_.close;
    }

    k->opi   = q_.opi;
    k->close = q_.close;
    k->high  = std::max( k->high, k->close );
    k->low   = std::min( k->low, k->close );
}

NVX_NS_END
