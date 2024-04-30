#ifndef B1DEF22C_4715_4F7E_BA26_C2C5DEA96D2D
#define B1DEF22C_4715_4F7E_BA26_C2C5DEA96D2D
#include <cstddef>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <map>
#include <mutex>
#include <stdio.h>
#include <sys/un.h>

#include "models.h"
#include "msg.h"
#include "proxy.h"

#define DC_ADDR "unix://tmp/datacenter"

NVX_NS_BEGIN

struct DcClient : IMarket {
    DcClient( IData* data_ );

    int start() override;
    int stop() override;
    int subscribe( const code_t& code_ ) override;
    int unsubscribe( const code_t& code_ ) override;

    int run();

private:
    void on_msg( const Msg* m_ );
    void on_ack( mid_t req_, char rc_ );
    void on_tick( const quotation_t& qut_ );

private:
    static void read_cb( struct bufferevent* bev, void* ctx );
    static void event_cb( struct bufferevent* bev, void* ctx );

private:
    using book_t = std::map<code_t, bool>;

    bool _dc_running = false;
    // book_t _allsubs;//这会导致加锁，非常不划算

private:
    struct bufferevent* _bev = nullptr;
    // std::mutex          _mtx;
};

NVX_NS_END

#endif /* B1DEF22C_4715_4F7E_BA26_C2C5DEA96D2D */
