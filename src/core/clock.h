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

#ifndef B87362DF_37FD_4B6A_9F1E_768AAFEA5563
#define B87362DF_37FD_4B6A_9F1E_768AAFEA5563
#include <chrono>
#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/util.h>
#include <functional>
#include <map>
#include <mutex>
#include <time.h>
#include <vector>

#include "definitions.h"
#include "ns.h"

NVX_NS_BEGIN

struct Clock final {
    using clockcb_t = std::function<void( exid_t, const datetime_t )>;

    static Clock& instance();

    void   attach( clockcb_t cb_, exid_t ex_ = -1 );
    void   tune( const datetime_t& dt_, exid_t ex_ );
    time_t now( exid_t ex_ = -1 );

private:
    void start();
    void handle_tick();

private:
    struct event& timer() { return _timer; }
    static void   fired( evutil_socket_t fd, short event_, void* arg );
    struct event  _timer;
    Clock();

private:
    struct notify_t {
        clockcb_t callback;
        exid_t    ex;
    };

    std::map<exid_t, time_t> _drifts;
    std::vector<notify_t>    _notif;
    std::mutex               _mutex;
};

NVX_NS_END

#define CLOCK NVX_NS::Clock::instance()

#endif /* B87362DF_37FD_4B6A_9F1E_768AAFEA5563 */
