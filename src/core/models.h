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

#ifndef B7C02AA3_83E0_4A71_B207_3E05F194B663
#define B7C02AA3_83E0_4A71_B207_3E05F194B663
#include <algorithm>
#include <math.h>

#include "definitions.h"
#include "ns.h"

NVX_NS_BEGIN

enum class variety_t {
    cn_fu,
    cn_stock,
    bitcoin,
};

struct funds {
    money    withdraw;    // 取款
    money    cashin;      // 存钱
    money    balance;     // 余额
    money    prebalance;  // 上次结算余额
    money    available;   // 可用资金
    money    cprofit;     // 平仓利润
    money    pprofit;     // 持仓利润
    money    commission;
    money    margin;
    money    lever;
    money    premargin;
    money    predeposit;
    money    precredit;
    money    deposit;
    datetime day;
};

struct candle {
    id_t     id;
    price    high;
    price    low;
    price    close;
    price    open;
    vol      volume;
    vol      opi;  // opi
    datetime time;

    // todo no need
    // code     symbol;

    price body() { return fabs( close - open ); }
    price long_shadow() { return std::max( uppers(), lowers() ); }
    price height() { return high - low; }
    price bar_mid() { return ( high + low ) / 2; }
    price body_mid() { return ( open + close ) / 2; }
    price figure() { return close; }
    price uppers() { return high - body_up(); }
    price lowers() { return body_low() - low; }
    price body_up() { return std::max( close, open ); }
    price body_low() { return std::min( close, open ); }

    bool red() { return close > open; }
    bool white() { return red(); }
    bool black() { return close < open; }
    bool green() { return black(); }
    bool doji() { return fabs( close - open ) < 1e-4; }
    bool embrace( candle& o ) { return body_up() >= o.body_up() && body_low() <= o.body_low(); }
    bool close_beyond() { return close > ( high + low ) / 2; }
    bool close_under() { return close < ( high + low ) / 2; }
    bool step_beyond( const candle& oth_ ) { return high > oth_.high && low > oth_.low && close > oth_.close; }
    bool step_below( const candle& oth_ ) { return low < oth_.low && close < oth_.close && high < oth_.high; }

// k线的开始时间，主要是调试用，确保K线分割合理正确
#ifdef CUB_DEBUG
    int sday;
    int stm;
#endif
};

#define PRICE_DEPTH 5

// real time ticks
struct tick {
    code     symbol;
    int      ex;        // 交易所
    vol      volume;    // 成交仓位
    money    turnover;  // 成交额
    vol      opi;
    vol      bidvol;  // 没有深度数据
    vol      askvol;
    price    bid;
    price    ask;
    price    highest;
    price    lowest;
    price    avgprice;    // 均价
    price    upperlimit;  // 涨跌停
    price    lowerlimit;
    price    last;  // 上次成交的价格
    price    open;
    price    close;
    datetime time;
};

enum class ord_dir {
    none,
    p_long,
    p_short,
    sell,
    cover,
};

enum class ord_type {
    limit,
    stop,
    cond,    // 条件单-似乎有点复杂，tbd
    fak,     // fill and kill,限价立即成交其余撤单
    market,  // 市价成交
    fok,     // fill or kill,限价立刻成交，否则撤单
    wok,     // 限价等5秒，不能成交立即撤单,WAIT OR KILL==普通限价单
    pursue,  // 限价不能成交就提高1个tick追赶5次，然后撤单==循环的FAK
    fam,     // 立即成交，其余使用市价交易 = fak+market
};

enum class ord_status {
    // todo noneed, create          = 0x0001,
    pending         = 0x0002,
    partial         = 0x0004,
    dealt           = 0x0008,
    deleted         = 0x0010,
    closed          = 0x0040,
    cancelling      = 0x0080,
    finished        = 0x0100,  // ctp 已成交和finish是两个状态，参照onrtntrade的函数说明
    aborted         = 0x0200,  // 操作失败
    cancelled       = deleted,
    partial_dealed  = partial | dealt,
    patial_deleted  = partial | deleted,
    patial_canelled = patial_deleted,
    error           = 0x0100
};

struct order {
    order() = default;
    order( oid id_, const code& c_, const vol v_, const price p_, const ord_type& t_, const ord_dir& d_ );
    oid        id       = NVX_BAD_OID;    //! 订单id
    code       symbol   = "";             //! 代码，RB1910
    exch       ex       = "";             //! 交易所，SHEX
    price      limit    = .0;             //! 期望成交价格，已成交价格
    price      tp_price = .0;             //! 止盈价格
    price      sl_price = .0;             //! 止损价格
    vol        qty      = .0;             //! 期望成交数量, 已成交数量
    vol        traded   = .0;             //! 已经成交
    ord_dir    dir      = ord_dir::none;  //! 方向，买、卖、平
    ord_status status   = ord_status::pending;
    ord_type   mode     = ord_type::market;
    bool       today    = true;             //! 今仓，昨仓
    datetime   dt       = datetime::now();  //! 成交或者下单的时间、日期
    xstring    remark   = "#";              //! 如果会非常频繁的创建和拷贝订单，这里最好是用数组--string的实现必须健壮,考虑到各种可能的诡异操作~
};

struct order_update {
    oid        id;
    vol        qty;
    price      dealt;
    ord_dir    dir;
    ord_status status;
};

struct pos_item {
    code    symbol;
    price   dealt;
    price   last_price;
    money   profit;
    money   close_profit;
    money   value;
    vol     qty;
    ord_dir dir;

    datetime update_time;
};

struct performance {
    int   records;  // 交易多少次
    money profit;   // 总利润盈利多少
    money commission;

    int   long_n;
    money long_profit;
    money long_loss;

    int    short_n;
    double short_profit;
    double short_loss;

    float win_rate;
    float profit_rate;
    money avg_pftlss;  // proft/lss
    money avg_pft;
    money avg_lss;

    money max_loss;
    money max_profit;
    int   profit_n;  // 亏损次数
    int   loss_n;    // 盈利次数

    array_t<money> profits;  // 每次有平仓都记录一下利润，用于画曲线
};

struct margin_rate {
    xstring broker;
    xstring investor;
    code    instrument;
    exch    ex;
    char    investor_range;   // ctp 投资者范围
    bool    is_relative;      // 是否相对交易所收取
    bool    hedge;            // 投机套保标志
    float   long_by_money;    /// 多头保证金率
    float   long_by_volume;   // 多头保证金费
    float   short_by_money;   /// 空头保证金率
    float   short_by_volume;  // 空头保证金费;
};

//---inlines
inline order::order( oid id_, const code& c_, const vol v_, const price p_, const ord_type& t_, const ord_dir& d_ ) {
    id     = id_;
    symbol = c_;
    qty    = v_;
    limit  = p_;
    mode   = t_;
    dir    = d_;
}

NVX_NS_END

#endif /* B7C02AA3_83E0_4A71_B207_3E05F194B663 */
