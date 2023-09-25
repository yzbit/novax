#include <algorithm>
#include <comm/log.hpp>

#include "reactor.h"

CUB_NS_BEGIN

#define REACTOR_XPUB "inproc://cub-xpub.chan"
#define REACTOR_XSUB "inproc://cub-xsub.chan"
#define REACTOR_DATA "inproc://cub-data.chan"
#define REACTOR_TRADE "inproc://cub-trade.chan"

struct LocalSocket {
    LocalSocket( zmq::context_t& ctx_, zmq::socket_type type_ )
        : sock( ctx_, type_ ) {
    }

    zmq::socket_t* operator->() {
        return &sock;
    }

    void connect( const char* addr_ ) {
        if ( connected ) return;

        // todo  connect 是有时间的,如果立刻ub消息,可能会导致第一条消息丢失, 可以等welcome msg之后再发送
        // https://github.com/zeromq/libzmq/issues/2267--zmq的welcome msg可能就是因为这个bug添加的
        sock.connect( addr_ );
        // todo verify ..
        std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
    }

    bool          connected = false;
    zmq::socket_t sock;
};

zmq::socket_t& Reactor::therad_safe_pub() {
    static thread_local LocalSocket _lsock( _center_ctx, zmq::socket_type::pub );
    _lsock.connect( "tcp://localhost:6666" );
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

    zmq::const_buffer buff{ data_, length_ };
    // fprintf( stderr, "before send" );
    zmq::send_result_t rc = sock.send( buff, zmq::send_flags::none );

    fprintf( stderr, "send ok sock = 0x%lx ,rc=%lu\n", ( int64_t )&sock, rc.has_value() ? rc.value() : 0 );

    return rc.has_value() ? 0 : -1;
}

Reactor& Reactor::instance() {
    static Reactor r;
    return r;
}

Reactor::Reactor() {
    // init();
}

void Reactor::init_svc() {
    _data.init( REACTOR_DATA );
    _trade.init( REACTOR_TRADE );
}

int Reactor::init() {
    init_svc();

    _center_ctx = std::move( zmq::context_t( 2 ) );

    auto mainloop = [ & ]() {
        zmq::socket_t xpub = zmq::socket_t( _center_ctx, zmq::socket_type::xpub );
        xpub.bind( "tcp://*:5555" );
        xpub.set( zmq::sockopt::xpub_verbose, 1 );

        zmq::socket_t xsub = zmq::socket_t( _center_ctx, zmq::socket_type::xsub );
        xsub.bind( "tcp://*:6666" );

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

    chan.bind( endpoint.c_str() );
}

void Reactor::filter_from_id( FilterToken& filter_, const msg::mid_t& id_ ) {
    unsigned uid   = ( unsigned )id_;
    int      index = 0;

    while ( uid ) {
        filter_[ index++ ] = uid & 0xFF;
        LOG_TRACE( "%02X ", filter_[ index - 1 ] );
        fprintf( stderr, "%02X ", filter_[ index - 1 ] );
        uid >>= 8;
    }
}

int Reactor::sub( const MsgIdSet& msg_set_, MsgHandler h_ ) {
    LOG_INFO( "prepare to recv messages." );
    static constexpr int kMaxMessgageLength = 1204;

    auto loop = [ & ]( zmq::socket_t& sock_ ) {
        std::unique_ptr<char[]> m = std::make_unique<char[]>( kMaxMessgageLength );

        zmq::mutable_buffer rbuff{ m.get(), kMaxMessgageLength };  // todo :是按照消息来边界做接收 吗 ?

        msg::Header* header = reinterpret_cast<msg::Header*>( m.get() );
        while ( 1 ) {
            auto rc = sock_.recv( rbuff, zmq::recv_flags::none );
            if ( !rc.has_value() ) {
                printf( "bad receive\n" );
                LOG_INFO( "recv error" );
                header->id = msg::mid_t::exception;
            }
            else {
                // fprintf( stderr, "good receive %lu\n", rc.value().size );
                LOG_INFO( "got message" );
            }

            h_( *header );
        }  // while(1)
    };

    std::thread( [ = ]() {
        zmq::socket_t chan;
        zmq::socket_t sub( _center_ctx, ZMQ_SUB );
        sub.set( zmq::sockopt::subscribe, "" );
        sub.connect( "tcp://localhost:5555" );  // connect to xpub
        loop( sub );

#if 0

        fprintf( stderr, "create sub thread\n" );

        if ( msg_set_.size() == 1 && *msg_set_.begin() == msg::mid_t::svc_data ) {
            fprintf( stderr, "svc data sub\n" );

            zmq::socket_t d = zmq::socket_t( _data.context, zmq::socket_type::pair );
            d.connect( REACTOR_DATA );
            chan.swap( d );
        }
        else if ( msg_set_.size() == 1 && *msg_set_.begin() == msg::mid_t::svc_order ) {

            fprintf( stderr, "create svc-trade sub thread\n" );
            zmq::socket_t t = zmq::socket_t( _trade.context, zmq::socket_type::pair );
            t.connect( REACTOR_TRADE );
            chan.swap( t );
        }
        else {
            fprintf( stderr, "create comm sub thread\n" );
            zmq::socket_t sub( _center_ctx, ZMQ_SUB );
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

        fprintf( stderr, "into loop\n" );
        loop( chan );
        fprintf( stderr, "loop finished\n" );
        chan.close();

#endif
    } ).detach();
    return 0;
}

Reactor::~Reactor() {
}

CUB_NS_END