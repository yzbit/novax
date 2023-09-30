#include <comm/datetime.h>
#include <comm/definitions.h>
#include <cub_ns.h>

CUB_NS_BEGIN
struct order_t {
    enum class dir_t {
        p_long,
        p_short,
        sell,
        cover,
    };

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

    oid_t    id;        //! 订单id
    code_t   code;      //! 代码，RB1910
    ex_t     ex;        //! 交易所，SHEX
    dir_t    dir;       //! 方向，买、卖、平
    price_t  price;     //! 期望成交价格，已成交价格
    vol_t    quantity;  //! 期望成交数量, 已成交数量
    status_t status;
    bool     today;     //! 今仓，昨仓
    DateTime datetime;  //! 成交或者下单的时间、日期
    string_t remark;    //! 如果会非常频繁的创建和拷贝订单，这里最好是用数组--string的实现必须健壮,考虑到各种可能的诡异操作~
};

CUB_NS_END