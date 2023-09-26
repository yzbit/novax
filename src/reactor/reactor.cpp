#include <algorithm>
#include <comm/log.hpp>

#include "reactor.h"

CUB_NS_BEGIN

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

zmq::socket_t& Reactor::therad_safe_pub() {
    static thread_local LocalSocket _lsock( _center_ctx, zmq::socket_type::pub );
    _lsock.connect( REACTOR_XSUB );
    return _lsock.sock;
}

zmq::socket_t& Reactor::distribute( const msg::mid_t& id_ ) {
    switch ( id_ ) {
    case msg::mid_t::svc_data:
        return _data.chan;
    case msg::mid_t::svc_order:
        return _trade.chan;
    default:
        return therad_safe_pub();
    }
}

int Reactor::pub( const void* data_, size_t length_ ) {
    const msg::Header* h = static_cast<const msg::Header*>( data_ );

    auto& sock = distribute( h->id );

    zmq::const_buffer  buff{ data_, length_ };
    zmq::send_result_t rc = sock.send( buff, zmq::send_flags::dontwait );
    if ( !rc.has_value() ) {
        LOG_INFO( "send fail. sock = 0x%lx", &sock );
    }

    return rc.has_value() ? 0 : -1;
}

Reactor& Reactor::instance() {
    static Reactor r;
    return r;
}

Reactor::Reactor() {
    init();
}

void Reactor::init_svc() {
    LOG_INFO( "##init svc" );
    _data.init( REACTOR_DATA );
    _trade.init( REACTOR_TRADE );
}

int Reactor::init() {
    init_svc();

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

Reactor::Svc::Svc( const std::string& endpoint_ ) {
    init( endpoint_ );
}

Reactor::Svc::~Svc() {
    chan.close();
    context.close();
}

void Reactor::Svc::init( const std::string& endpoint_ ) {
    endpoint = endpoint_;

    context = std::move( zmq::context_t( 1 ) );
    chan    = std::move( zmq::socket_t( context, zmq::socket_type::pair ) );

    assert( context );
    assert( chan );

    LOG_INFO( "context=0x%lx ,sock=0x%lx, endpoint=%s", ( intptr_t )&context, ( intptr_t )&chan, endpoint_.c_str() );
    chan.bind( endpoint.c_str() );
}

void Reactor::filter_from_id( FilterToken& filter_, const msg::mid_t& id_ ) {
    unsigned uid   = ( unsigned )id_;
    int      index = 0;

    while ( uid ) {
        filter_[ index++ ] = uid & 0xFF;
        LOG_TRACE( "%02X ", filter_[ index - 1 ] );
        uid >>= 8;
    }
}

int Reactor::sub( const MsgIdSet& msg_set_, MsgHandler h_ ) {
    LOG_INFO( "prepare to recv messages." );

    // warn capture h_ by value
    auto loop = [ = ]( zmq::socket_t& sock_ ) {
        std::unique_ptr<char[]> m = std::make_unique<char[]>( msg::kMaxMsgLength );

        zmq::mutable_buffer rbuff{ m.get(), msg::kMaxMsgLength };  // todo :是按照消息来边界做接收 吗 ?

        msg::Header* header = reinterpret_cast<msg::Header*>( m.get() );
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
        zmq::socket_t chan;

        LOG_INFO( "create sub thread" );

        if ( msg_set_.size() == 1 && *msg_set_.begin() == msg::mid_t::svc_data ) {
            LOG_INFO( "svc data sub" );

            // note 从源代码可以看到，inproc属于single connection，后续的连接会被忽略
            zmq::socket_t d = zmq::socket_t( _data.context, zmq::socket_type::pair );
            d.connect( REACTOR_DATA );
            chan.swap( d );
        }
        else if ( msg_set_.size() == 1 && *msg_set_.begin() == msg::mid_t::svc_order ) {
            LOG_INFO( "create svc-trade sub " );
            zmq::socket_t t = zmq::socket_t( _trade.context, zmq::socket_type::pair );
            t.connect( REACTOR_TRADE );
            chan.swap( t );
        }
        else {
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

            chan.swap( sub );
        }

        LOG_INFO( "into loop" );
        loop( chan );
        LOG_INFO( "loop finished" );
        chan.close();
    } ).detach();
    return 0;
}

Reactor::~Reactor() {
}

CUB_NS_END