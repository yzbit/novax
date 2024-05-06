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

#include <mutex>
#include <stdio.h>

#include "data_center.h"

#include "msg.h"

NVX_NS_BEGIN
DcClient::DcClient( IData* data_ )
    : IMarket( data_ ) {
}

int DcClient::start() {
    return send_msg( _bev, StartDcMsg() );
}

int DcClient::stop() {
    return send_msg( _bev, StopDcMsg() );
}

int DcClient::subscribe( const code_t& code_ ) {
    SubMsg s;
    s.code = code_;

    //    {
    //      std::unique_lock<std::mutex> _lck{ _mtx };
    //    _allsubs.emplace( code_, false );
    //}
    return send_msg( _bev, s );
}

int DcClient::unsubscribe( const code_t& code_ ) {
    UnsubMsg um;
    um.code = code_;

    //    {
    //        std::unique_lock<std::mutex> _lck{ _mtx };
    //        _allsubs.erase( code_ );
    //   }

    return send_msg( _bev, um );
}

void DcClient::on_msg( const Msg* m_ ) {
    switch ( m_->id ) {
    default: return;
    case nvx::mid_t::ack:
        return on_ack( reinterpret_cast<const AckMsg*>( m_ )->req, reinterpret_cast<const AckMsg*>( m_ )->rc );
    case nvx::mid_t::data_tick:
        return on_tick( reinterpret_cast<const QutMsg*>( m_ )->qut );
    }
}

void DcClient::on_ack( mid_t req_, char rc_ ) {
    // if ( req_ == mid_t::sub_data && 0 == rc_ ) {
    // }
    if ( rc_ != 0 ) {
        printf( "req failed,error=%d\n", rc_ );
    }
}

void DcClient::on_tick( const quotation_t& qut_ ) {
    delegator()->update( qut_ );
}

void DcClient::read_cb( struct bufferevent* bev_, void* ctx ) {
    auto msg = recv_msg( bev_ );
    if ( !msg ) return;

    DcClient* cli = reinterpret_cast<DcClient*>( ctx );
    cli->on_msg( msg );
}

void DcClient::event_cb( struct bufferevent* bev, short event_, void* ctx ) {
    printf( "dcclient on event" );
}

int DcClient::run() {
    struct event_base* base = event_base_new();

    struct sockaddr_un addr;
    memset( &addr, 0, sizeof( addr ) );
    addr.sun_family = AF_UNIX;
    strcpy( addr.sun_path, DC_SERVER_ADDR );

    int sock = socket( AF_UNIX, SOCK_STREAM, 0 );
    ::connect( sock, ( struct sockaddr* )&addr, sizeof( addr ) );

    struct bufferevent* bev = bufferevent_socket_new( base, sock, BEV_OPT_CLOSE_ON_FREE );

    bufferevent_setcb( bev, &DcClient::read_cb, nullptr, &DcClient::event_cb, this );
    bufferevent_enable( bev, EV_READ | EV_WRITE );

    bufferevent_write( bev, "subscribe", 9 );

    return event_base_dispatch( base );
}

NVX_NS_END