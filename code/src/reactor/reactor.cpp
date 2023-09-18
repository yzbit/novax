
#include <log.hpp>

#include "reactor.h"

CUB_NS_BEGIN

Reactor& Reactor::instance() {
    static Reactor r;
    return r;
}

int Reactor::pub( const Msg& msg_ ) {
    int rc = ( int )_publisher->send( &msg_, sizeof( msg_ ), 0 );
    return rc;
}

int Reactor::sub( const MsgIdSet& msg_set_, MsgHandler h_ ) {
    zmq::context_t context( 1 );
    zmq::socket_t  subsock( context, ZMQ_SUB );

    LOG_INFO( "connect pub @local 5555" );
    subsock.connect( "tcp://localhost:5555" );

    if ( msg_set_.empty() ) {
        subsock.setsockopt( ZMQ_SUBSCRIBE, "", 0 );
    }
    else {
        for ( const auto& id : msg_set_ ) {
            char id_str[ 8 ] = { 0 };
            sprintf( id_str, "%04d", id );

            LOG_INFO( "sub: %s", id_str );

            subsock.setsockopt( ZMQ_SUBSCRIBE, id_str, strlen( id_str ) );
        }
    }

    Msg m;
    while ( 1 ) {
        auto rc = subsock.recv( &m, sizeof( m ) );
        if ( rc == -1 ) {
            LOG_INFO( "recv error" );
            h_( { kExceptMsg, 0 } );
        }
        else {
            LOG_INFO( "got message" );
            h_( m );
        }
    }

    //zmq_close(subscriber);
    //zmq_ctx_destroy(context);
    return 0;
}

Reactor : ~Reactor() {
    //zmq_close(subscriber);
    //zmq_ctx_destroy(context);
}

int Reactor::init() {
    _pub_context = std::make_unique<zmq::context_t>( 1 );

    _publisher = std::make_unique<zmq::socket_t>( *_pub_context, ZMQ_PUB );
    _publisher->bind( "tcp://*:5555" );

    //一个pub可以绑定到多个节点
    //_publisher->bind( "ipc://msg.ipc" );

    return 0;
}

CUB_NS_END