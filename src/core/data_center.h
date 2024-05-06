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

#define DC_SERVER_ADDR "unix://tmp/datacenter"

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
    static void event_cb( struct bufferevent* bev, short event_, void* ctx );

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
