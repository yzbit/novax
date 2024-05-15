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

struct DcPub : IPub {
    DcPub( IData* d_ )
        : _srv( d_ ) {
    }

    const quotation_t& qut_from_pub( const pub::tick_msg_t& m_ ) const {
        return m_;
    }

    int post( const pub::msg_t& m_ ) override {
        if ( m_.type() != pub::msg_type::tick ) return -1;

        _srv->update( qut_from_pub( m_.get<pub::tick_msg_t>() ) );
        return 0;
    }

private:
    IData* _srv = nullptr;
};

void DcServer::update( const quotation_t& tick_ ) {
    update_subs();

    for ( auto& sub : _subs ) {
        // todo,send
    }
}

void DcServer::on_event( const dc::Event* m_ ) {
    switch ( m_->id ) {
    default: break;
    case dc::event_t::sub_data:
        // todo, get sockete ?
        _candicates.enqueue( { "rb", 0 } );
        break;
    }
}

nvx_st DcServer::persist( const quotation_t& tick_ ) {
    //_cache.
    return NVX_OK;
}

void DcServer::thread_save( DcServer& s_ ) {
    for ( ;; )
        ;
}

nvx_st DcServer::run() {
    // todo
    auto                      pub   = new DcPub( this );
    [[maybe_unused]] IMarket* ctpmd = new ctp::CtpExMd( pub );
    // todo
   // [[maybe_unused]] auto fut = std::async( std::launch::async, &DcServer::thread_save, *this );

    attach( 0 );
    return NVX_OK;
}

void DcServer::update_subs() {
    sub_t s;
    while ( _candicates.try_dequeue( s ) ) {
        // todo 去重
        _subs.push_back( s );
    }
}
NVX_NS_END