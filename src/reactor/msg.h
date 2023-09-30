#ifndef D0A82F45_7141_4EE2_ABB6_9929837DA41C
#define D0A82F45_7141_4EE2_ABB6_9929837DA41C
#include <cub_ns.h>
#include <stdint.h>

CUB_NS_BEGIN

namespace msg {
enum class mid_t : int32_t {
    exception       = -1,
    insuficent_room = -2,

    svc_data = 1,
    svc_order,
    svc_data_cmd,
    svc_trade_cmd,

    clock_time,  //! 系统时钟
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

constexpr int kMaxMsgLength = 1024;

#define COMPOSE_MSG( MsgDataTick )

struct header_t {
    mid_t  id;
    size_t length;
};

struct DataTick {
    int debug;
};

template <typename T>
struct msg_t {
    header_t h;
    T        body;
};

using DataTickFrame = msg_t<DataTick>;

#pragma pack()

template <typename T>
const T& frame_cast( const header_t& h ) {
    return reinterpret_cast<const T&>( h );
}

}  // namespace msg

CUB_NS_END

#endif /* D0A82F45_7141_4EE2_ABB6_9929837DA41C */
