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
#include <future>
#include <map>
#include <signal.h>
// #include <stdio.h>
#include <thread>
#include <time.h>
#include <unordered_map>

#include "clock.h"
#include "log.hpp"

NVX_NS_BEGIN

Clock::Clock() {
    //  [[maybe_unused]] auto fut = std::async( std::launch::async, &Clock::start, this );
    //  printf( "task started\n" );
    std::thread( [ & ]() { this->start(); } ).detach();
}

Clock& Clock::instance() {
    static Clock c;
    return c;
}

void Clock::attach( clockcb_t cb_, exid_t ex_ ) {
    std::unique_lock<std::mutex> lock( _mutex );
    _notif.push_back( { cb_, ex_ } );
}

void Clock::tune( const datetime_t& dt_, exid_t ex_ ) {
    if ( !ex_ ) return;

    LOG_TAGGED( "clock", "tune clock, exid=%d, local=%s, exp=%s", ex_, datetime_t().from_unix_time( time( 0 ) ).to_iso().c_str(), dt_.to_iso().c_str() );
    {
        std::unique_lock<std::mutex> lock( _mutex );
        _drifts.try_emplace( ex_, dt_.to_unix_time() - time( 0 ) );
        // LOG_TAGGED( "clock", "ex clock drift %u", _drifts[ ex_ ] );
    }
}

time_t Clock::now( exid_t ex_ ) {
    if ( ex_ < 0 ) return time( 0 );

    std::unique_lock<std::mutex> lock( _mutex );
    return _drifts.count( ex_ ) > 0 ? _drifts[ ex_ ] + time( 0 ) : time( 0 );
}

void Clock::handle_tick() {
    std::unique_lock<std::mutex> lock( _mutex );
    for ( auto& v : _notif ) {
        if ( _drifts.count( v.ex ) > 0 )
            v.callback( v.ex, datetime_t().from_unix_time( time( 0 ) + _drifts[ v.ex ] ) );
        else
            v.callback( -1, datetime_t().from_unix_time( time( 0 ) ) );
    }
}

void Clock::fired( evutil_socket_t fd, short event_, void* arg ) {
    // printf( "clck fired\n" );

    Clock* c = ( Clock* )arg;
    c->handle_tick();

    struct timeval one_sec = { 1, 0 };
    evtimer_add( &c->timer(), &one_sec );
}

void Clock::start() {
    struct event_base* base = event_base_new();

    event_assign( &timer(), base, -1, EV_PERSIST, &Clock::fired, this );

    struct timeval one_sec = { 1, 0 };
    evtimer_add( &timer(), &one_sec );

    event_base_dispatch( base );
    event_base_free( base );
}

NVX_NS_END