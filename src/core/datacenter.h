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

#ifndef B1DEF22C_4715_4F7E_BA26_C2C5DEA96D2D
#define B1DEF22C_4715_4F7E_BA26_C2C5DEA96D2D
#include <cstddef>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <event2/thread.h>
#include <map>
#include <mutex>
#include <stdio.h>
#include <sys/un.h>

#include "concurrentqueue.h"
#include "models.h"
#include "proxy.h"
#include "pub.h"
#include "ringbuffer.h"

#define DC_SERVER_ADDR "unix://tmp/datacenter"

NVX_NS_BEGIN
namespace dc {
enum class event_t : int8_t {
    exception       = -1,
    insuficent_room = -2,

    start_dc,
    stop_dc,
    sub_data,
    unsub_data,
    ack,
    data_tick
};

struct AckEvent {
    event_t id = event_t::ack;
    event_t req;
    char    rc;
};

struct QutEvent {
    event_t id = event_t::data_tick;
    tick    qut;
};

struct SubEvent {
    event_t id = event_t::sub_data;
    code    code;
};

struct StopDcEvent {
    event_t id = event_t::stop_dc;
};

struct StartDcEvent {
    event_t id = event_t::start_dc;
};

struct UnsubEvent {
    event_t id = event_t::unsub_data;
    code    code;
};

// 实际发送消息的时候没必要发送固定长度的Msg，那是浪费，是什么消息就发送什么消息即可，根据id把消息转成对应下msg即可
// 问题是如何知道消息长度呢,如何根据消息id判断消息长度，最好是静态的，否则recv消息的时候可能是截断的？
// 如何保证消息收到的是完整的
// msg消息可以无损的转成任何xxMsg，前提是数据收的很全
union Event {
    event_t      id;
    StartDcEvent startdc;  // todo
    StopDcEvent  stopdc;   // todo
    SubEvent     sub;
    UnsubEvent   unsub;
    QutEvent     qut;
};

#define MAX_EVENT_LEN 1024
struct EventHdr {
    short len;
    char  data[ MAX_EVENT_LEN ];
};

template <typename M>
int send_event( struct bufferevent* bev_, const M& m_ ) {
    static EventHdr h;

    h.len = sizeof( M );
    memcpy( h.data, &m_, h.len );

    return bufferevent_write( bev_, &h, sizeof( short ) + h.len );
}

inline const Event* recv_event( struct bufferevent* bev_ ) {
    static EventHdr h;

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

    return reinterpret_cast<Event*>( &h );
}
}  // namespace dc

struct endpoint {
    endpoint();
    virtual ~endpoint() {}

protected:
    void attach( struct bufferevent* bev_ );

private:
    virtual void on_event( const dc::Event* m_, struct bufferevent* bev ) = 0;
    virtual void on_ack( dc::event_t req_, char rc_ )                     = 0;

private:
    static void read_cb( struct bufferevent* bev, void* ctx );
    static void event_cb( struct bufferevent* bev, short event_, void* ctx );

private:
    struct bufferevent* _bev = nullptr;
};

struct dc_client : market, endpoint {
    dc_client( pub* data_ );

    nvx_st start() override;
    nvx_st stop() override;
    nvx_st subscribe( const code& code_ ) override;
    nvx_st unsubscribe( const code& code_ ) override;
    nvx_st run();

private:
    void on_event( const dc::Event* m_, struct bufferevent* bev ) override;
    void on_ack( dc::event_t req_, char rc_ ) override;
    void on_tick( const tick& qut_ );

private:
    // using book_t = std::map<code, bool>;

    bool _dc_running = false;

private:
    struct bufferevent* _bev = nullptr;
};

struct sub_t {
    ins_t code;
    void* socket;
};

struct dc_server : endpoint {
    void   update( const tick& tick_ );
    nvx_st run();

private:
    void on_event( const dc::Event* m_, struct bufferevent* bev ) override;

private:
    void        update_subs();
    nvx_st      persist( const tick& );  // todo save to file
    nvx_st      start_server();
    static void accept_cb( evutil_socket_t listener, short event, void* arg );

private:
    static void thread_save( dc_server& s_ );

private:
#define MAX_CACHE_CNT 128

    moodycamel::ConcurrentQueue<sub_t> _candicates;
    std::vector<sub_t>                 _subs;
    ring_buff<tick, MAX_CACHE_CNT>     _cache;
};

NVX_NS_END

#endif /* B1DEF22C_4715_4F7E_BA26_C2C5DEA96D2D */
