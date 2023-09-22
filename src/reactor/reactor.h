#ifndef C20EEFC2_0BE4_4918_AAD4_2F0119D413CB
#define C20EEFC2_0BE4_4918_AAD4_2F0119D413CB
#include <comm/ns.h>
#include <functional>
#include <memory>
#include <set>
#include <string>
#include <zmq.hpp>

#include "msg.h"

CUB_NS_BEGIN

struct BindingEnd {
    const char* pub;
    const char* sub;
};

#define DECL_BINDDING( _name_, _pub_, _sub_ ) constexpr BindingEnd _name_ = { _pub_, _sub_ }

DECL_BINDDING( TOPIC_DATA_BINDING, "tcp://*:5010", "tcp://localhost:5010" );

struct Publisher final {
    Publisher( const std::string& id_ );
    ~Publisher();

private:
    std::string                     _id;
    std::unique_ptr<zmq::socket_t>  _sock;
    std::unique_ptr<zmq::context_t> _ctx;
};

struct Reactor {
    using MsgIdSet   = std::set<msg::mid_t>;
    using MsgHandler = std::function<void( const msg::Header& h_ )>;

    static Reactor& instance();

    ~Reactor();
    Reactor();

    int init();

    template <typename T>
    int pub( const T& m_ ) {
        return pub( &m_, sizeof( T ) );
    }

    int pub( void* data_, size_t length_ );
    int sub( const MsgIdSet& msg_set_, MsgHandler h_ );

private:
    std::unique_ptr<zmq::socket_t>  _publisher;
    std::unique_ptr<zmq::context_t> _pub_context;
};

CUB_NS_END

#define REACTOR CUB_NS::Reactor::instance()

#endif /* C20EEFC2_0BE4_4918_AAD4_2F0119D413CB */
