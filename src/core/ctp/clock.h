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

#ifndef D9686528_C6CC_4311_BC7C_6A8CECBCFCBD
#define D9686528_C6CC_4311_BC7C_6A8CECBCFCBD

#include <ctp/ThostFtdcMdApi.h>
#include <map>

#include "../clock.h"
#include "../definitions.h"
#include "../ns.h"
#include "comm.h"

NVX_NS_BEGIN

namespace ctp {
struct Clock : clock {
    Clock();

    static Clock& get_clock() {
        static Clock _c;
        return _c;
    }

    time_t now( const exid& ex_ ) override;
    void   tune( const datetime& dt_, exid ex_ );
    void   reset( CThostFtdcRspUserLoginField* pRspUserLogin_ );

private:
    std::map<exid, time_t> _drifts;
};

inline Clock::Clock() {
    _drifts.try_emplace( ( exid )extype_t::SHFE, 0 );
    _drifts.try_emplace( ( exid )extype_t::DCE, 0 );
    _drifts.try_emplace( ( exid )extype_t::FFEX, 0 );
    _drifts.try_emplace( ( exid )extype_t::INE, 0 );
    _drifts.try_emplace( ( exid )extype_t::GFEX, 0 );
    _drifts.try_emplace( ( exid )extype_t::CZCE, 0 );
}

time_t Clock::now( const exid& ex_ ) {
    return _drifts.count( ex_ ) > 0 ? _drifts[ ex_ ] + time( 0 ) : time( 0 );
}

void Clock::tune( const datetime& dt_, exid ex_ ) {
    if ( _drifts.count( ex_ ) <= 0 ) {
        LOG_INFO( "can not tune clock of exid=%d", ex_ );
        return;
    }

    _drifts[ ex_ ] = dt_.to_unix_time() - time( 0 );
}

void Clock::reset( CThostFtdcRspUserLoginField* f_ ) {
    datetime dt;

    dt.from_ctp( f_->TradingDay, f_->SHFETime, 0 );
    tune( dt, ( int )extype_t::SHFE );

    dt.from_ctp( f_->TradingDay, f_->DCETime, 0 );
    tune( dt, ( int )extype_t::DCE );

    dt.from_ctp( f_->TradingDay, f_->CZCETime, 0 );
    tune( dt, ( int )extype_t::CZCE );

    dt.from_ctp( f_->TradingDay, f_->FFEXTime, 0 );
    tune( dt, ( int )extype_t::FFEX );

    dt.from_ctp( f_->TradingDay, f_->INETime, 0 );
    tune( dt, ( int )extype_t::INE );

    dt.from_ctp( f_->TradingDay, f_->GFEXTime, 0 );
    tune( dt, ( int )extype_t::GFEX );
}
}  // namespace ctp

NVX_NS_END

#define CTP_CLOCK NVX_NS::ctp::Clock::get_clock()

#endif /* D9686528_C6CC_4311_BC7C_6A8CECBCFCBD */
