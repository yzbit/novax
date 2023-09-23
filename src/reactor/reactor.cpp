#include <algorithm>
#include <comm/log.hpp>

#include "reactor.h"

CUB_NS_BEGIN

//"tcp://*:5555" );
Publisher::Publisher( const BindingEnd& binding_ )
    : _binding( binding_ ) {
    _ctx  = std::make_unique<zmq::context_t>( 1 );
    _sock = std::make_unique<zmq::socket_t>( *_ctx, ZMQ_PUB );

    LOG_INFO( "create pub with id=%s", id_.c_str() );

    // 一个pub可以绑定到多个节点
    //_publisher->bind( "ipc://msg.ipc" );
    _sock->bind( binding_.pub );
}

bool Publisher::serve( const MsgIdSet& ids_ ) {
    return std::find_first_of( _binding.topics.begin(), _binding.topics.end(), ids_.begin(), ids_.end() )
           != _binding.topics.end();
}

int Publisher::attach( zmq::socket_t& sub_ ) {
    sub_.connect( _binding.sub );
    return 0;
}

Publisher::~Publisher() {
    _sock->close();
    _ctx->close();
}

Reactor& Reactor::instance() {
    static Reactor r;
    return r;
}

Reactor::Reactor() {
    //  init();
}

int Publisher::send( const void* msg_, size_t length_ ) {
    zmq::const_buffer buff{ msg_, length_ };
    // zmq::send_result_t rc =
    _sock->send( buff, zmq::send_flags::none );

    return 0;
}

bool Publisher::serve( const msg::mid_t& id_ ) {
    return _binding.topics.count( id_ ) > 0;
}

int Reactor::pub( void* data_, size_t length_ ) {
    // if ( msg::mid_t::data_tick == mid ) {
    //
    //}
    msg::Header* h = static_cast<msg::Header*>( data_ );

    for ( auto& p : _pubs ) {
        if ( p->serve( h->id ) ) {
            return p->send( data_, length_ );
        }
    }

    return 0;
}

int Reactor::sub( const MsgIdSet& msg_set_, MsgHandler h_ ) {
    LOG_INFO( "prepare to recv messages." );
    static constexpr int kMaxMessgageLength = 1204;

    std::thread( [ = ]() {
        zmq::context_t context( 1 );
        zmq::socket_t  subsock( context, ZMQ_SUB );

        if ( msg_set_.empty() ) {
            subsock.set( zmq::sockopt::subscribe, "" );
            for ( auto& p : _pubs ) {
                p->attach( subsock );
            }
        }
        else {
            for ( auto& id : msg_set_ ) {
                char id_str[ 8 ] = { 0 };
                sprintf( id_str, "%04x", id );
                fprintf( stderr, id_str );

                LOG_INFO( "sub: %s", id_str );
                // subsock.set( zmq::sockopt::subscribe, id_str );
                subsock.set( zmq::sockopt::subscribe, "\00\04" );
            }
        }

        for ( auto& p : _pubs ) {
            LOG_INFO( "connect pub @local 5555" );
            if ( p->serve( msg_set_ ) ) {
                p->attach( subsock );
            }
        }

        std::unique_ptr<char[]> m = std::make_unique<char[]>( kMaxMessgageLength );

        zmq::mutable_buffer rbuff{ m.get(), kMaxMessgageLength };  // todo :是按照消息来边界做接收 吗 ?
        while ( 1 ) {
            auto rc = subsock.recv( rbuff, zmq::recv_flags::none );
            if ( !rc.has_value() ) {
                LOG_INFO( "recv error" );
                // h_( { kExceptMsg, 0 } );
            }
            else {
                LOG_INFO( "got message" );
                // h_( m );
            }
        }  // while(1)

        subsock.close();
        context.close();
    } ).detach();
    return 0;
    return 0;
}

Reactor::~Reactor() {
}

int Reactor::init() {
    return 0;
}

CUB_NS_END