#ifndef B7C02AA3_83E0_4A71_B207_3E05F194B663
#define B7C02AA3_83E0_4A71_B207_3E05F194B663
#include <algorithm>
#include <math.h>

#include "datetime.h"
#include "definitions.h"
#include "ns.h"

CUB_NS_BEGIN
struct fund_t {
    money_t    withdraw;    // 取款
    money_t    cashin;      // 存钱
    money_t    balance;     // 余额
    money_t    prebalance;  // 上次结算余额
    money_t    available;   // 可用资金
    money_t    cprofit;     // 平仓利润
    money_t    pprofit;     // 持仓利润
    money_t    commission;
    money_t    margin;
    money_t    lever;
    money_t    premargin;
    money_t    predeposit;
    money_t    precredit;
    money_t    deposit;
    datetime_t day;
};

struct candle_t {
    id_t       id;
    price_t    high;
    price_t    low;
    price_t    close;
    price_t    open;
    vol_t      volume;
    vol_t      opi;  // opi
    code_t     symbol;
    datetime_t time;

    price_t body() { return fabs( close - open ); }
    price_t long_shadow() { return std::max( uppers(), lowers() ); }
    price_t height() { return high - low; }
    price_t bar_mid() { return ( high + low ) / 2; }
    price_t body_mid() { return ( open + close ) / 2; }
    price_t price() { return close; }
    price_t uppers() { return high - body_up(); }
    price_t lowers() { return body_low() - low; }
    price_t body_up() { return std::max( close, open ); }
    price_t body_low() { return std::min( close, open ); }

    bool red() { return close > open; }
    bool white() { return red(); }
    bool black() { return close < open; }
    bool green() { return black(); }
    bool doji() { return fabs( close - open ) < 1e-4; }
    bool embrace( candle_t& o ) { return body_up() >= o.body_up() && body_low() <= o.body_low(); }
    bool close_beyond() { return close > ( high + low ) / 2; }
    bool close_under() { return close < ( high + low ) / 2; }
    bool step_beyond( const candle_t& oth_ ) { return high > oth_.high && low > oth_.low && close > oth_.close; }
    bool step_below( const candle_t& oth_ ) { return low < oth_.low && close < oth_.close && high < oth_.high; }

// k线的开始时间，主要是调试用，确保K线分割合理正确
#ifdef CUB_DEBUG
    int sday;
    int stm;
#endif
};

#define PRICE_DEPTH 5

// real time ticks
struct quotation_t {
    code_t  code;
    vol_t   volume;    // 成交仓位
    money_t turnover;  // 成交额
    vol_t   opi;

    int     depth;
    vol_t   bidvol[ PRICE_DEPTH ];
    vol_t   askvol[ PRICE_DEPTH ];
    price_t bid[ PRICE_DEPTH ];
    price_t ask[ PRICE_DEPTH ];

    price_t    highest;
    price_t    lowest;
    price_t    avgprice;    // 均价
    price_t    upperlimit;  // 涨跌停
    price_t    lowerlimit;
    price_t    last;  // 上次成交的价格
    price_t    open;
    price_t    close;
    datetime_t time;
};

enum class dir_t {
    p_long,
    p_short,
    sell,
    cover,
};

struct order_t {
    enum class status_t {
        create          = 0x0001,
        pending         = 0x0002,
        patial          = 0x0004,
        dealed          = 0x0008,
        deleted         = 0x0010,
        closed          = 0x0040,
        cancelled       = deleted,
        patial_dealed   = patial | dealed,
        patial_deleted  = patial | deleted,
        patial_canelled = patial_deleted,
        error           = 0x0100
    };

    oid_t      id;        //! 订单id
    code_t     code;      //! 代码，RB1910
    ex_t       ex;        //! 交易所，SHEX
    dir_t      dir;       //! 方向，买、卖、平
    price_t    price;     //! 期望成交价格，已成交价格
    vol_t      quantity;  //! 期望成交数量, 已成交数量
    status_t   status;
    bool       today;     //! 今仓，昨仓
    datetime_t datetime;  //! 成交或者下单的时间、日期
    string_t   remark;    //! 如果会非常频繁的创建和拷贝订单，这里最好是用数组--string的实现必须健壮,考虑到各种可能的诡异操作~
};

struct position_t {
    code_t     symbol;
    price_t    price;
    dir_t      direction;
    vol_t      open_qty; /*负数表示做空，就不需要方向字段*/
    vol_t      close_qty;
    money_t    open_amt;
    money_t    close_amt;
    money_t    used_margin;
    vol_t      margin_vol;  // by qty or by amount
    money_t    margin_amt;
    vol_t      position;       // 今仓--按照合约查询，今仓左仓可能合并查到的
    vol_t      last_position;  // 昨仓
    money_t    commission;
    money_t    close_profit;  // close profit
    money_t    open_cost;     // 开仓成本
    money_t    cost;          // 持仓成本，目前还不知道区别
    money_t    profit;        // pzt profit
    money_t    frz_margin;    // 冻结的现金和保证金
    money_t    frz_cash;
    string_t   magic;
    datetime_t datetime;
};

struct performance_t {
    int     records;  // 交易多少次
    money_t profit;   // 总利润盈利多少
    money_t commission;

    int     long_n;
    money_t long_profit;
    money_t long_loss;

    int    short_n;
    double short_profit;
    double short_loss;

    float   win_rate;
    float   profit_rate;
    money_t avg_pftlss;  // proft/lss
    money_t avg_pft;
    money_t avg_lss;

    money_t max_loss;
    money_t max_profit;
    int     profit_n;  // 亏损次数
    int     loss_n;    // 盈利次数

    array_t<money_t> profits;  // 每次有平仓都记录一下利润，用于画曲线
};

struct margin_rate_t {
    string_t broker;
    string_t investor;
    code_t   instrument;
    ex_t     ex;
    char     investor_range;   // ctp 投资者范围
    bool     is_relative;      // 是否相对交易所收取
    bool     hedge;            // 投机套保标志
    float    long_by_money;    /// 多头保证金率
    float    long_by_volume;   // 多头保证金费
    float    short_by_money;   /// 空头保证金率
    float    short_by_volume;  // 空头保证金费;
};

CUB_NS_END

#endif /* B7C02AA3_83E0_4A71_B207_3E05F194B663 */
