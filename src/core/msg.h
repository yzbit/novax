#ifndef D0A82F45_7141_4EE2_ABB6_9929837DA41C
#define D0A82F45_7141_4EE2_ABB6_9929837DA41C
#include "ns.h"
#include <stdint.h>

SATURN_NS_BEGIN

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

// todo 不考虑跨平台和分布式的情况，否则很多数据结构需要定义两套：一套是不带pack的，一套是专门给msg使用的
// 以后看情况，如果有必要再做重构
// #pragma pack( 1 )

constexpr int kMaxMsgLength = 1024;

#define COMPOSE_MSG( MsgDataTick )

struct header_t {
    mid_t  id;
    size_t length;
};

template <typename T, mid_t ID>
struct msg_t {
    using PayloadType = T;

    msg_t() {
        h.id     = ID;
        h.length = sizeof( T );
    }

    msg_t( const T& p_ )
        : msg_t() {
        payload = p_;
    }

    PayloadType& body() { return payload; }

    header_t h;
    T        payload;
};

#define DECL_MESSAGE( _name_, _payload_, _id_ ) \
    struct _name_ : msg_t<_payload_, _id_> {    \
        _name_( const _payload_& p_ )           \
            : msg_t( p_ ) {}                    \
        _name_() = default;                     \
        msg_t::PayloadType* operator->() {      \
            return &payload;                    \
        }                                       \
    }

// #pragma pack()

template <typename T>
const T& frame_cast( const header_t& h ) {
    return reinterpret_cast<const T&>( h );
}

}  // namespace msg

SATURN_NS_END

#endif /* D0A82F45_7141_4EE2_ABB6_9929837DA41C */
