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

* \author: yaozn(zinan@outlook.com) , qianq(695997058@qq.com)
* \date: 2024
**********************************************************************************/

#include <future>
#include <mutex>
#include <stdio.h>

#include "ctp/mdproxy.h"
#include "datacenter.h"
#include "pub.h"

NVX_NS_BEGIN

struct dc_pub : ipub {
    dc_pub( dc_server* d_ )
        : _srv( d_ ) {
    }

    const tick& qut_from_pub( const pub::tick_msg& m_ ) const {
        return m_;
    }

    int post( const pub::msg& m_ ) override {
        if ( m_.type() != pub::msg_type::tick ) return -1;

        _srv->update( qut_from_pub( m_.get<pub::tick_msg>() ) );
        return 0;
    }

private:
    dc_server* _srv = nullptr;
};

void dc_server::update( const tick& tick_ ) {
#if 0
    update_subs();
    persist( tick_ );

    if ( find( tick_.code ) ) {
        for ( auto& sub : _subs ) {
            // todo,send
        }
    }
#endif
}

void dc_server::on_event( const dc::event* m_, struct bufferevent* bev_ ) {
    switch ( m_->id ) {
    default: break;
    case dc::event_t::sub_data: {
        const dc::sub_event* sub = reinterpret_cast<const dc::sub_event*>( m_ );
        _candicates.enqueue( { sub->c, bev_ } );
    } break;
    case dc::event_t::unsub_data: {
        const dc::sub_event* sub = reinterpret_cast<const dc::sub_event*>( m_ );
        _candicates.enqueue( { sub->c, 0 } );
    } break;
    }
}

nvx_st dc_server::persist( const tick& tick_ ) {
    //_cache.
    return NVX_OK;
}

void dc_server::thread_save( dc_server& s_ ) {
    for ( ;; )
        ;
}

nvx_st dc_server::run() {
    // todo
    auto                     pub   = new dc_pub( this );
    [[maybe_unused]] market* ctpmd = new ctp::mdex( pub );
    // todo
    // [[maybe_unused]] auto fut = std::async( std::launch::async, &dc_server::thread_save, *this );

    attach( 0 );
    return NVX_OK;
}

void dc_server::update_subs() {
    subs s;
    while ( _candicates.try_dequeue( s ) ) {
        if ( !s.socket ) {
            _subs.erase( std::remove_if( _subs.begin(), _subs.end(), [ & ]( const auto& v ) { return v.code == s.code; } ) );
        }
        else {
            _subs.push_back( s );
        }
    }
}

void dc_server::accept_cb( evutil_socket_t listener, short event, void* arg ) {
    struct event_base* base = ( struct event_base* )arg;

    struct sockaddr_un client_addr;
    socklen_t          client_addr_len = sizeof( client_addr );

    evutil_socket_t client_sock = accept( listener, ( struct sockaddr* )&client_addr, &client_addr_len );

    if ( client_sock == -1 ) {
        perror( "accept" );
        return;
    }

    evutil_make_socket_nonblocking( client_sock );

    struct bufferevent* bev = bufferevent_socket_new( base, client_sock, BEV_OPT_CLOSE_ON_FREE );

    // 设置读写回调

    event_base_dispatch( base );
}

nvx_st dc_server::start_server() {
#if 0
    struct sockaddr_un addr;
    memset( &addr, 0, sizeof( addr ) );
    addr.sun_family = AF_UNIX;
    strcpy( addr.sun_path, DC_SERVER_ADDR );

    int sock = socket( AF_UNIX, SOCK_STREAM, 0 );
    evutil_make_listen_socket_reuseable( sock );

    unlink( DC_SERVER_ADDR );
    bind( sock, ( struct sockaddr* )&addr, sizeof( addr ) );
    listen( sock, 5 );

    struct event_base* base = event_base_new();

    struct event* listener = evutil_make_socket_nonblocking( sock );

    event_set( listener, sock, EV_READ | EV_PERSIST, accept_cb, base );
    event_add( listener, NULL );

    event_base_dispatch( base );

    close( sock );
#endif
    return NVX_OK;
}

NVX_NS_END