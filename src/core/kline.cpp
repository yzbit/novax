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

#include "kline.h"

#include "clock.h"
#include "log.hpp"

#define BAR_TRACK 0
NVX_NS_BEGIN

kline::kline( const code& code_, const period& p_, size_t series_count_ )
    : _symbol( code_ )
    , _period( p_ )
    , _bars( new bar_series( series_count_ ) ) {
    normalize();
}

candle& kline::bar( int index_ ) {
    return *( _bars->get( index_ ) );
}

period kline::cycle() const {
    return _period;
}

void kline::normalize() {
    if ( _period.b == period::base::hour ) {
        _period.b = period::base::min;
        _period.r *= 60;
    }
    else if ( _period.b == period::base::week ) {
        _period.b = period::base::day;
        _period.r *= 5;
    }
    // 月份和年份最好是使用自然月和自然年;分钟和小时严格按照尺度而不是自然日分割
}

const code& kline::symbol() const {
    return _symbol;
}

const tick& kline::qut() const {
    return _qut;
}

size_t kline::count() const {
    return _bars->size();
}

kline::stamp kline::qut_stamp( const tick& q_ ) {
    return {
        .day  = ( unsigned )q_.time.d.year * 10'000 + ( unsigned )q_.time.d.month * 100 + ( unsigned )q_.time.d.day,
        .time = ( unsigned )q_.time.t.hour * 10'000'000 + ( unsigned )q_.time.t.minute * 100'000 + ( unsigned )q_.time.t.second * 1000 + ( unsigned )q_.time.t.milli
    };
}

//--period如果是天，当然是以每天的开盘价作为上一个x线的结束, 否则当前k一致都是动态的
//--以小时为单位，则必然是以开盘+n小时作为结果，而且以天为单位
//--暂且不处理超过日的周期
bool kline::is_new_bar( const tick& q_ ) {
    stamp s = qut_stamp( q_ );

    if ( ( _period.b == period::base::min && ( _lst_stamp.day != s.day || _lst_stamp.time / 100'000 != s.time / 100'000 ) )
         || ( _period.b == period::base::second && ( _lst_stamp.day != s.day || _lst_stamp.time / 1000 != s.time / 1000 ) )
         || ( _period.b == period::base::hour && ( _lst_stamp.day != s.day || _lst_stamp.time / 10'000'000 != s.time / 10'000'000 ) )
         || ( _period.b == period::base::day && _lst_stamp.day != s.day )
         || ( _period.b == period::base::milli && ( _lst_stamp.day != s.day || _lst_stamp.time != s.time ) )
         || ( _period.b == period::base::month && _lst_stamp.day / 100 != s.day / 100 )
         || ( _period.b == period::base::year && _lst_stamp.day / 10'000 != s.day / 10'000 ) ) {
        _gathered = ( _gathered + 1 ) >= _period.r
                        ? 0
                        : _gathered + 1;
    }
    return _gathered == 0;
}

#define IS_VALID_BAR( k ) ( k && k->time.is_valid() )
void kline::update( const tick& q_ ) {
    // 此时可以认为是新开盘
    if ( abs( CLOCK.now( q_.ex ) - q_.time.to_unix_time() ) > 3 * 60 ) {
        LOG_INFO( "market open ;obsolete data recieved: %u %u", CLOCK.now( q_.ex ), q_.time.to_unix_time() );
        return;
    }

    if ( !_bar || is_new_bar( q_ ) ) {
        if ( IS_VALID_BAR( _bar ) ) {
            _bar->time   = q_.time;
            _bar->volume = q_.volume - _bar->volume;
        }

        _bar = _bars->advance();

        _bar->time   = q_.time;
        _bar->volume = q_.volume;
        _bar->open   = q_.close;
    }

    _bar->opi   = q_.opi;
    _bar->close = q_.close;
    _bar->high  = std::max( _bar->high, _bar->close );
    _bar->low   = std::min( _bar->low, _bar->close );

    //--
    _lst_stamp = qut_stamp( q_ );
}

NVX_NS_END
