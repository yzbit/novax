#include <algorithm>
#include <thread>

#include "reactor_impl.h"

#include "log.hpp"
#include "ns.h"

SATURN_NS_BEGIN

#define REACTOR_XPUB "inproc://cub-xpub-chan"
#define REACTOR_XSUB "inproc://cub-xsub-chan"
#define REACTOR_DATA "inproc://cub-data.chan"
#define REACTOR_TRADE "inproc://cub-trade.chan"

struct LocalSocket {
    LocalSocket( zmq::context_t& ctx_, zmq::socket_type type_ )
        : sock( ctx_, type_ ) {
        LOG_INFO( "local sock created, ctx=0x%lx, sock=0x%lx", ( intptr_t )&ctx_, ( intptr_t )&sock );
    }

    zmq::socket_t* operator->() {
        return &sock;
    }

    void connect( const char* addr_ ) {
        if ( connected ) return;

        // warn:  connect 是有时间的,如果立刻ub消息,可能会导致第一条消息丢失,welcome msg 只给sub用的，这里简单的等待2ms
        // https://github.com/zeromq/libzmq/issues/2267--zmq的welcome msg可能就是因为这个bug添加的
        sock.connect( addr_ );
        connected = true;

        // todo verify ..
        std::this_thread::sleep_for( std::chrono::milliseconds( 2 ) );
    }

    bool connected = false;
    // zmq::context_t context;
    zmq::socket_t sock;
};

zmq::socket_t& ReactorImpl::therad_safe_pub() {
    static thread_local LocalSocket _lsock( _center_ctx, zmq::socket_type::pub );
    _lsock.connect( REACTOR_XSUB );
    return _lsock.sock;
}

zmq::socket_t& ReactorImpl::distribute( const msg::mid_t& id_ ) {
    return therad_safe_pub();
}

int ReactorImpl::pub( const void* data_, size_t length_ ) {
    const msg::header_t* h = static_cast<const msg::header_t*>( data_ );

    auto& sock = distribute( h->id );

    zmq::const_buffer  buff{ data_, length_ };
    zmq::send_result_t rc = sock.send( buff, zmq::send_flags::dontwait );
    if ( !rc.has_value() ) {
        LOG_INFO( "send fail. sock = 0x%lx", &sock );
    }

    return rc.has_value() ? 0 : -1;
}

Reactor& Reactor::instance() {
    static ReactorImpl r;
    return r;
}

ReactorImpl::ReactorImpl() {
    init();
}

int ReactorImpl::init() {
    _center_ctx = std::move( zmq::context_t( 2 ) );

    auto mainloop = [ & ]() {
        zmq::socket_t xpub = zmq::socket_t( _center_ctx, zmq::socket_type::xpub );
        xpub.bind( REACTOR_XPUB );
        xpub.set( zmq::sockopt::xpub_verbose, 1 );

        zmq::socket_t xsub = zmq::socket_t( _center_ctx, zmq::socket_type::xsub );
        xsub.bind( REACTOR_XSUB );

        // http://api.zeromq.org/3-2:zmq-proxy
        // When the frontend is a ZMQ_XSUB socket, and the backend is a ZMQ_XPUB socket,
        // the proxy shall act as a message forwarder that collects messages from a set
        // of publishers and forwards these to a set of subscribers
        zmq::proxy( xsub, xpub );
    };

    std::thread( mainloop ).detach();

    return 0;
}

void ReactorImpl::filter_from_id( FilterToken& filter_, const msg::mid_t& id_ ) {
    unsigned uid   = ( unsigned )id_;
    int      index = 0;

    while ( uid ) {
        filter_[ index++ ] = uid & 0xFF;
        LOG_TRACE( "%02X ", filter_[ index - 1 ] );
        uid >>= 8;
    }
}

int ReactorImpl::sub( const mid_set_t& msg_set_, msg_handler_t h_ ) {
    LOG_INFO( "prepare to recv messages." );

    // warn capture h_ by value
    auto loop = [ = ]( zmq::socket_t& sock_ ) {
        std::unique_ptr<char[]> m = std::make_unique<char[]>( msg::kMaxMsgLength );

        zmq::mutable_buffer rbuff{ m.get(), msg::kMaxMsgLength };  // todo :是按照消息来边界做接收 吗 ?

        msg::header_t* header = reinterpret_cast<msg::header_t*>( m.get() );
        while ( 1 ) {
            auto rc = sock_.recv( rbuff, zmq::recv_flags::none );
            if ( !rc.has_value() ) {
                LOG_INFO( "recv error" );
                header->id = msg::mid_t::exception;
            }
            else {
                if ( rc.value().size != rc.value().untruncated_size ) {
                    LOG_INFO( "!!NOT enough buffer" );
                    header->id = msg::mid_t::insuficent_room;
                }
            }

            h_( *header );
        }
    };

    std::thread( [ = ]() {
        LOG_INFO( "create sub thread" );
        zmq::socket_t sub( _center_ctx, ZMQ_SUB );

        LOG_INFO( "create comm sub thread: context=0x%lx, sock=0x%lx", ( intptr_t )&_center_ctx, ( intptr_t )&sub );
        if ( msg_set_.empty() ) {
            sub.set( zmq::sockopt::subscribe, "" );
        }
        else {
            for ( auto& id : msg_set_ ) {
                if ( id == msg::mid_t::svc_data || id == msg::mid_t::svc_order ) {
                    LOG_INFO( "cant book svc_data/svc_order" );
                    continue;
                }

                FilterToken filter = { 0 };
                filter_from_id( filter, id );
                sub.set( zmq::sockopt::subscribe, filter );
            }
        }
        sub.connect( REACTOR_XPUB );

        LOG_INFO( "into loop" );
        loop( sub );
        LOG_INFO( "loop finished" );
        sub.close();
    } ).detach();
    return 0;
}

ReactorImpl::~ReactorImpl() {
}

SATURN_NS_END