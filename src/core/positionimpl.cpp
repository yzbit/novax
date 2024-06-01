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

#include "positionimpl.h"

NVX_NS_BEGIN

position_impl::position_impl( const code& code_, bool long_ ) {
    _p.symbol = code_;
    _long     = long_;

    reset();
}

void position_impl::reset() {
    memset( &_p, 0x00, sizeof( pos_item_t ) );
}

nvx_st position_impl::stop( vol qty_, price price_ ) {
}

nvx_st position_impl::profit( vol qty_, price price_ ) {
}

price position_impl::avg_dealt() {
}

vol position_impl::qty() {
}

kidx position_impl::last_entry() {
}

kidx position_impl::last_exit() {
}

vol position_impl ::herge( vol qty_, price price_ ) {
    vol act_closed = std::min( p->qty, update_.qty );

    if ( p->qty <= act_closed )
        LOG_INFO( "over sell/cover of qty:" );

    // if(qty==0) 可以移除了，但是为了计算累计的平仓利润，暂且保留
    //  成交总价值，首先确保p->qty> =0;

    p->qty -= act_closed;
    // todo
    [[maybe_unused]] money close_value = update_.price * act_closed;

    p->close_profit += ( src_.dir == ord_dir::sell
                             ? update_.price - p->price  // 卖平仓的利润是卖出价格 - 买入价格
                             : p->price - update_.price )
                       * act_closed;

    if ( p->qty == 0 ) {
        LOG_INFO( "position closed: %s", p->symbol.c_str() );
        p->price = 0;  // else keep
    }

    p->value      = p->price * p->qty;
    p->last_price = update_.price;
}

// 所以我们的最好做法是把每个合约的仓位统一成一条，然后算出平均价，每次有成交的时候就简单的处理就好,否则还要区分昨仓，今仓
// 如果这样就会出现合约同时持有long和short，也就是说一个合约应该有两条记录，[0] long汇总 [1]short汇总
void position_impl::accum( vol qty_, price price_ ) {

    p->qty += update_.traded;
    p->price      = ( src_.qty * src_.price + update_.traded * update_.price ) / p->qty;
    p->last_price = update_.price;
}

NVX_NS_END