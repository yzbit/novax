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

#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/util.h>
#include <time.h>

struct clock {
    void handle_tick() {
        time_t     cur_time;
        struct tm* loctime;

        time( &cur_time );
        loctime = localtime( &cur_time );

        printf( "Timer triggered at %s\n", asctime( loctime ) );
    }

    static void timer_cb( evutil_socket_t fd, short event_, void* arg ) {
        clock* c = ( clock* )( arg );
        c->handle_tick();

        struct timeval one_sec = { 0, 500000 };
        evtimer_add( &c->timer(), &one_sec );
    }

    struct event& timer() { return _timer; }

private:
    struct event _timer;
};

int main() {
    struct clock c;

    struct event_base* base = event_base_new();

    // struct event* timer = evtimer_new( base, timer_cb, NULL );
    event_assign( &c.timer(), base, -1, EV_PERSIST, &clock::timer_cb, &c );

    struct timeval one_sec = { 0, 500000 };
    evtimer_add( &c.timer(), &one_sec );

    event_base_dispatch( base );
    event_base_free( base );

    return 0;
}