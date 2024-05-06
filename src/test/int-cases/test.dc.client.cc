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