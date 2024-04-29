#include <event2/event.h>
#include <time.h>

void timer_cb( evutil_socket_t fd, short event, void* arg ) {

    time_t     cur_time;
    struct tm* loctime;

    time( &cur_time );
    loctime = localtime( &cur_time );

    printf( "Timer triggered at %s\n", asctime( loctime ) );
}

int main() {

    struct event_base* base = event_base_new();

    struct event* timer = evtimer_new( base, timer_cb, NULL );

    struct timeval one_sec = { 1, 0 };
    evtimer_add( timer, &one_sec );

    event_base_dispatch( base );

    return 0;
}