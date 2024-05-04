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