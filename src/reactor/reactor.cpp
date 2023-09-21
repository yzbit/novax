
#include <comm/log.hpp>

#include "reactor.h"

CUB_NS_BEGIN

Reactor& Reactor::instance() {
    static Reactor r;
    return r;
}

Reactor::Reactor() {
    //  init();
}

int Reactor::pub( const Msg& msg_ ) {
    zmq::const_buffer buff{ &msg_, sizeof( msg_ ) };

    //zmq::send_result_t rc =
    _publisher->send( buff, zmq::send_flags::none );
    return 0;
}

int Reactor::sub( const MsgIdSet& msg_set_, MsgHandler h_ ) {
    LOG_INFO( "prepare to recv messages." );
    std::thread( [ = ]() {
        zmq::context_t context( 1 );
        zmq::socket_t  subsock( context, ZMQ_SUB );

        subsock.set( zmq::sockopt::immediate, false );

        LOG_INFO( "connect pub @local 5555" );
        subsock.connect( "tcp://localhost:5555" );

        if ( msg_set_.empty() ) {
            subsock.set( zmq::sockopt::subscribe, "" );
        }
        else {
            for ( auto& id : msg_set_ ) {
                char id_str[ 8 ] = { 0 };
                sprintf( id_str, "%04x", id );
                fprintf( stderr, id_str );

                LOG_INFO( "sub: %s", id_str );
                //subsock.set( zmq::sockopt::subscribe, id_str );
                subsock.set( zmq::sockopt::subscribe, "\00\04" );
            }
        }

        Msg m;

        zmq::mutable_buffer rbuff{ &m, sizeof( m ) };
        while ( 1 ) {
            auto rc = subsock.recv( rbuff, zmq::recv_flags::none );
            if ( !rc.has_value() ) {
                LOG_INFO( "recv error" );
                // h_( { kExceptMsg, 0 } );
            }
            else {
                LOG_INFO( "got message" );
                h_( m );
            }
        }  //while(1)

        subsock.close();
        context.close();
    } ).detach();

    return 0;
}

Reactor::~Reactor() {
    _publisher->close();
    _pub_context->close();
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