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

#include <mutex>
#include <stdio.h>

#include "datacenter.h"

NVX_NS_BEGIN
DcClient::DcClient( IPub* p_ )
    : IMarket( p_ ) {
}

nvx_st DcClient::start() {
    return send_event( _bev, dc::StartDcEvent() );
}

nvx_st DcClient::stop() {
    return send_event( _bev, dc::StopDcEvent() );
}

nvx_st DcClient::subscribe( const code_t& code_ ) {
    dc::SubEvent s;
    s.code = code_;

    //    {
    //      std::unique_lock<std::mutex> _lck{ _mtx };
    //    _allsubs.emplace( code_, false );
    //}
    return send_event( _bev, s );
}

nvx_st DcClient::unsubscribe( const code_t& code_ ) {
    dc::UnsubEvent um;
    um.code = code_;

    //    {
    //        std::unique_lock<std::mutex> _lck{ _mtx };
    //        _allsubs.erase( code_ );
    //   }

    return send_event( _bev, um );
}

void DcClient::on_event( const dc::Event* m_ ) {
    switch ( m_->id ) {
    default: return;
    case dc::event_t::ack:
        return on_ack( reinterpret_cast<const dc::AckEvent*>( m_ )->req, reinterpret_cast<const dc::AckEvent*>( m_ )->rc );
    case dc::event_t::data_tick:
        return on_tick( reinterpret_cast<const dc::QutEvent*>( m_ )->qut );
    }
}

void DcClient::on_ack( dc::event_t req_, char rc_ ) {
    // if ( req_ == mid_t::sub_data && 0 == rc_ ) {
    // }
    if ( rc_ != 0 ) {
        printf( "req failed,error=%d\n", rc_ );
    }
}

void DcClient::on_tick( const quotation_t& qut_ ) {
    // todo
    // delegator()->update( qut_ );
}

void IEndpoint::read_cb( struct bufferevent* bev_, void* ctx ) {
    auto msg = dc::recv_event( bev_ );
    if ( !msg ) return;

    IEndpoint* cli = reinterpret_cast<IEndpoint*>( ctx );
    cli->on_event( msg );
}

void IEndpoint::event_cb( struct bufferevent* bev, short event_, void* ctx ) {
    printf( "dcclient on event" );
}

void IEndpoint::attach( struct bufferevent* bev_ ) {
    _bev = bev_;

    bufferevent_setcb( _bev, &IEndpoint::read_cb, nullptr, &IEndpoint::event_cb, this );
    bufferevent_enable( _bev, EV_READ | EV_WRITE );
}

IEndpoint::IEndpoint() {
}

nvx_st DcClient::run() {
    struct event_base* base = event_base_new();

    struct sockaddr_un addr;
    memset( &addr, 0, sizeof( addr ) );
    addr.sun_family = AF_UNIX;
    strcpy( addr.sun_path, DC_SERVER_ADDR );

    int sock = socket( AF_UNIX, SOCK_STREAM, 0 );
    ::connect( sock, ( struct sockaddr* )&addr, sizeof( addr ) );

    struct bufferevent* bev = bufferevent_socket_new( base, sock, BEV_OPT_CLOSE_ON_FREE );

    attach( bev );
    // bufferevent_setcb( bev, &DcClient::read_cb, nullptr, &DcClient::event_cb, this );
    // bufferevent_enable( bev, EV_READ | EV_WRITE );

    // bufferevent_write( bev, "subscribe", 9 );

    return event_base_dispatch( base );
}

NVX_NS_END