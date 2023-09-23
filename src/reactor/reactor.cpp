#include <algorithm>
#include <comm/log.hpp>

#include "reactor.h"

CUB_NS_BEGIN

#define REACTOR_XPUB "inproc://cub-xpub"
#define REACTOR_XSUB "inproc://cub-xsub"

struct LocalSocket {
    LocalSocket( zmq::context_t* ctx_, zmq::socket_type type_ )
        : sock( *ctx_, type_ ) {
    }

    zmq::socket_t* operator->() {
        return &sock;
    }

    void connect( const char* addr_ ) {
        if ( connected ) return;

        sock.connect( addr_ );
    }

    bool          connected = false;
    zmq::socket_t sock;
};

int Reactor::pub( const void* data_, size_t length_ ) {
    static thread_local LocalSocket _lsock( _context.get(), zmq::socket_type::pub );

    _lsock.connect( REACTOR_XPUB );

    zmq::const_buffer buff{ data_, length_ };

    zmq::send_result_t rc = _lsock->send( buff, zmq::send_flags::none );

    return rc.has_value() ? 0 : -1;
}

Reactor& Reactor::instance() {
    static Reactor r;
    return r;
}

Reactor::Reactor() {
    init();
}

int Reactor::init() {
    _context = std::make_unique<zmq::context_t>( 2 );

    auto mainloop = [ & ]() {
        zmq::socket_t xpub = zmq::socket_t( *_context.get(), zmq::socket_type::xpub );
        xpub.bind( REACTOR_XPUB );

        zmq::socket_t xsub = zmq::socket_t( *_context.get(), zmq::socket_type::xsub );
        xsub.bind( REACTOR_XSUB );

         zmq::proxy( xpub, xsub );
        //zmq::proxy( xsub, xpub );
    };

    std::thread( mainloop ).detach();

    return 0;
}

int Reactor::sub( const MsgIdSet& msg_set_, MsgHandler h_ ) {
    LOG_INFO( "prepare to recv messages." );
    static constexpr int kMaxMessgageLength = 1204;

    std::thread( [ = ]() {
        // zmq::context_t context( 1 );
        zmq::socket_t subsock( *_context.get(), ZMQ_SUB );

        if ( msg_set_.empty() ) {
            subsock.set( zmq::sockopt::subscribe, "" );
        }
        else {
            for ( auto& id : msg_set_ ) {
                char id_str[ 8 ] = { 0 };
                sprintf( id_str, "%04x", ( int )id );
                fprintf( stderr, id_str );

                LOG_INFO( "sub: %s", id_str );
                // subsock.set( zmq::sockopt::subscribe, id_str );
                // subsock.set( zmq::sockopt::subscribe, "\00\04" );
                subsock.set( zmq::sockopt::subscribe, "\01" );
            }
        }

        subsock.connect( REACTOR_XSUB );
        std::unique_ptr<char[]> m = std::make_unique<char[]>( kMaxMessgageLength );

        zmq::mutable_buffer rbuff{ m.get(), kMaxMessgageLength };  // todo :是按照消息来边界做接收 吗 ?
        while ( 1 ) {
            auto rc = subsock.recv( rbuff, zmq::recv_flags::none );
            if ( !rc.has_value() ) {
                printf( "bad receive\n" );
                LOG_INFO( "recv error" );
                // h_( { kExceptMsg, 0 } );
            }
            else {
                printf( "good receive\n" );
                LOG_INFO( "got message" );
                // h_( m );
            }
        }  // while(1)

        subsock.close();
    } ).detach();
    return 0;
}

Reactor::~Reactor() {
}

CUB_NS_END