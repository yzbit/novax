#include "portfolio.h"

CUB_NS_BEGIN

//这样 一来 order和position有啥区别....,都是全过程跟踪..要我说,成交了才叫仓位,不成交只是order
//也就是order最终会转化为仓位,但是只有实际成交后才算,而成交无非就是:开仓,平仓;
//其他的过程都属于订单管理,不应该在仓位管理里面,至于仓位的历史,理应是order的历史,和仓位有啥关系?"历史仓位"?
int Instrument::update( vol_t qty_, price_t price_, bool herge_ ) {

    return 0;
}

Portfolio& Portfolio::instance() {
    static Portfolio p;
    return p;
}

CUB_NS_END