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

#include "m_maker.h"

void MarketMaker::on_init( Context& c ) {
}

void MarketMaker::on_refresh( Context& c ) {
    price spread  = c.q.ask - c.q.bid;
    price percent = spread / c.q.ask * 100;
    price mprice  = ( c.q.ask + c.q.bid ) / 2;

    if ( percent > 0.1 && c.position() == 0 ) {
        buy_price  = c.q.bid + spread * 0.05;
        sell_price = c.q.ask - spread * 0.05;

        //下多条腿,还是分别下

        c.buylong( "btc/usdt", 0.001, buy_price, ord_type::fok );
        c.sellshort( "btc/usdt", 0.001, sell_price, ord_type::fok );
    }

    c.close();
}
