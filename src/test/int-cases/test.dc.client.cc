#include <event2/bufferevent.h>
#include <event2/event.h>
#include <stdio.h>
#include <sys/un.h>

#define SOCK_PATH "unix://tmp/datacenter"

void message_cb( struct bufferevent* bev, void* ctx ) {
    char buf[ 256 ];
    bufferevent_read( bev, buf, sizeof( buf ) );
    printf( "Received: %s\n", buf );
}
int main() {
    struct event_base* base = event_base_new();

    struct sockaddr_un addr;
    memset( &addr, 0, sizeof( addr ) );
    addr.sun_family = AF_UNIX;
    strcpy( addr.sun_path, SOCK_PATH );

    int sock = socket( AF_UNIX, SOCK_STREAM, 0 );
    connect( sock, ( struct sockaddr* )&addr, sizeof( addr ) );

    struct bufferevent* bev = bufferevent_socket_new(
        base, sock, BEV_OPT_CLOSE_ON_FREE );

    bufferevent_setcb( bev, message_cb, NULL, NULL, NULL );
    bufferevent_enable( bev, EV_READ | EV_WRITE );

    bufferevent_write( bev, "subscribe", 9 );

    event_base_dispatch( base );

    return 0;
}