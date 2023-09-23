#ifndef C20EEFC2_0BE4_4918_AAD4_2F0119D413CB
#define C20EEFC2_0BE4_4918_AAD4_2F0119D413CB
#include <array>
#include <comm/ns.h>
#include <functional>
#include <memory>
#include <set>
#include <string>
#include <zmq.hpp>

#include "msg.h"

CUB_NS_BEGIN

using MsgIdSet   = std::set<msg::mid_t>;
using MsgHandler = std::function<void( const msg::Header& h_ )>;

struct Reactor {
    static constexpr int kMaxPubCount = 16;

    static Reactor& instance();

    ~Reactor();
    Reactor();

    int init();

    template <typename T>
    int pub( const T& m_ ) {
        return pub( &m_, sizeof( T ) );
    }

    int pub( const void* data_, size_t length_ );
    int sub( const MsgIdSet& msg_set_, MsgHandler h_ );

private:
    std::unique_ptr<zmq::context_t> _context;
};

CUB_NS_END

#define REACTOR CUB_NS::Reactor::instance()

#endif /* C20EEFC2_0BE4_4918_AAD4_2F0119D413CB */
