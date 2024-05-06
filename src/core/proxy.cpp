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

#include "proxy.h"

#include "ctp/ctp_md_proxy.h"
#include "ctp/ctp_trade_proxy.h"
#include "data_center.h"

NVX_NS_BEGIN

ITrader::~ITrader() {}

IBroker::IBroker( ITrader* tr_ )
    : _tr( tr_ ) {
    tr_->_ib = this;
};

IBroker::~IBroker() {}

ITrader* IBroker::delegator() { return _tr; }

IData::~IData() {}

IMarket::IMarket( IData* dt_ )
    : _dt( dt_ ) {
    dt_->_m = this;
}

IMarket::~IMarket() {}

IData* IMarket::delegator() { return _dt; }

IMarket* create_market( IData* d_ ) {
    return new DcClient( d_ );
}

IBroker* create_broker( ITrader* t_ ) {
    return new ctp::CtpTrader( t_ );
}

NVX_NS_END