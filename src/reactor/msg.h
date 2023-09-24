#ifndef D0A82F45_7141_4EE1_ABB6_9929837DA41C
#define D0A82F45_7141_4EE1_ABB6_9929837DA41C
#include <comm/ns.h>
#include <stdint.h>

CUB_NS_BEGIN

namespace msg {
enum class mid_t : int32_t {
    exception = -1,

    svc_data = 1,
    svc_order,
    svc_data_cmd,
    svc_trade_cmd,

    data_tick = 100,
    book_data,
    unbook_data,

    put_order,
    cancel_order,
    close_order,
    del_order,
    order_update,

    qry_rt_position,
    qry_rt_cap,
    rt_position_update,
    rt_cap_update,

    cub_log
};

#pragma pack( 1 )

#define COMPOSE_MSG( MsgDataTick )
struct Header {
    mid_t  id;
    size_t length;
};

struct DataTick {
};

template <typename T>
struct Msg {
    Header h;
    T      body;
};

using DataTickFrame = Msg<DataTick>;

#pragma pack()
}  // namespace msg

CUB_NS_END

#endif /* D0A82F45_7141_4EE1_ABB6_9929837DA41C */
