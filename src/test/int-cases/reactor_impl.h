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

#ifndef C20EEFC2_0BE4_4918_AAD4_2F0119D413CB
#define C20EEFC2_0BE4_4918_AAD4_2F0119D413CB
#include <array>
#include <functional>
#include <memory>
#include <set>
#include <zmq.hpp>

#include "definitions.h"
#include "msg.h"
#include "ns.h"
#include "reactor.h"

NVX_NS_BEGIN

using FilterToken = char[ 4 ];

struct ReactorImpl : Reactor {
    static constexpr int kMaxPubCount = 16;

    ~ReactorImpl();
    ReactorImpl();

    int pub( const void* data_, size_t length_ ) override;
    int sub( const mid_set_t& msg_set_, msg_handler_t h_ ) override;

private:
    zmq::socket_t& distribute( const msg::mid_t& id_ );
    zmq::socket_t& therad_safe_pub();

    int init();

private:
    void filter_from_id( FilterToken& filter_, const msg::mid_t& ids_ );

private:
    zmq::conText _center_ctx;
};

NVX_NS_END

#endif /* C20EEFC2_0BE4_4918_AAD4_2F0119D413CB */
