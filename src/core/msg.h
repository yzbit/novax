/************************************************************************************
MIT License

Copyright (c) 2024 [YaoZinan zinan@outlook.com nvx-quant.com]

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

*@init: Yaozn
*@contributors:Yaozn
*@update: 2024
**********************************************************************************/

#ifndef D0A82F45_7141_4EE2_ABB6_9929837DA41C
#define D0A82F45_7141_4EE2_ABB6_9929837DA41C
#include <cstdint>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <stdint.h>

#include "models.h"
#include "ns.h"

NVX_NS_BEGIN

enum class mid_t : int8_t {
    exception       = -1,
    insuficent_room = -2,

    start_dc,
    stop_dc,
    sub_data,
    unsub_data,

    ack,

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

struct AckMsg {
    mid_t id = mid_t::data_tick;
    mid_t req;
    char  rc;
};

struct QutMsg {
    mid_t       id = mid_t::data_tick;
    quotation_t qut;
};

struct SubMsg {
    mid_t  id = mid_t::sub_data;
    code_t code;
};

struct StopDcMsg {
    mid_t id = mid_t::stop_dc;
};

struct StartDcMsg {
    mid_t id = mid_t::start_dc;
};

struct UnsubMsg {
    mid_t  id = mid_t::unsub_data;
    code_t code;
};

// 实际发送消息的时候没必要发送固定长度的Msg，那是浪费，是什么消息就发送什么消息即可，根据id把消息转成对应下msg即可
// 问题是如何知道消息长度呢,如何根据消息id判断消息长度，最好是静态的，否则recv消息的时候可能是截断的？
// 如何保证消息收到的是完整的
// msg消息可以无损的转成任何xxMsg，前提是数据收的很全
union Msg {
    mid_t      id;
    StartDcMsg startdc;
    StopDcMsg  stopdc;
    SubMsg     sub;
    UnsubMsg   unsub;
    QutMsg     qut;
};

#define MAX_MSG_LEN 1024
struct MsgHdr {
    short len;
    char  data[ MAX_MSG_LEN ];
};

template <typename M>
int send_msg( struct bufferevent* bev_, const M& m_ ) {
    static MsgHdr h;

    h.len = sizeof( M );
    memcpy( h.data, &m_, h.len );

    return bufferevent_write( bev_, &h, sizeof( short ) + h.len );
}

inline const Msg* recv_msg( struct bufferevent* bev_ ) {
    static MsgHdr h;

    size_t len = evbuffer_get_length( bufferevent_get_input( bev_ ) );
    if ( len < sizeof( short ) ) {
        return nullptr;
    }

    if ( sizeof( short ) != bufferevent_read( bev_, &h.len, sizeof( short ) ) ) {
        return nullptr;
    }

    if ( h.len > short( len - sizeof( short ) ) ) return nullptr;

    if ( h.len != ( short )bufferevent_read( bev_, h.data, h.len ) ) {
        return nullptr;
    }

    return reinterpret_cast<Msg*>( &h );
}

NVX_NS_END

#endif /* D0A82F45_7141_4EE2_ABB6_9929837DA41C */
