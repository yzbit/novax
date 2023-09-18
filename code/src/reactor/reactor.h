#ifndef C20EEFC2_0BE4_4918_AAD4_2F0119D413CB
#define C20EEFC2_0BE4_4918_AAD4_2F0119D413CB
#include <functional>
#include <memory>
#include <ns.h>
#include <set>
#include <zmq.hpp>

#include "msg.h"

CUB_NS_BEGIN

struct Reactor {
    using MsgIdSet   = std::set<mid_t>;
    using MsgHandler = std::function<void( const Msg& m_ )>;

    static Reactor& instance();

    int pub( const Msg& msg_ );
    int sub( const MsgIdSet& msg_set_, MsgHandler h_ );

private:
    int init();

private:
    std::unique_ptr<zmq::socket_t>  _publisher;
    std::unique_ptr<zmq::context_t> _pub_context;
};

CUB_NS_END

#define REACTOR CUB_NS::Reactor::instance()

#endif /* C20EEFC2_0BE4_4918_AAD4_2F0119D413CB */
