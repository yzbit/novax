#ifndef C20EEFC2_0BE4_4918_AAD4_2F0119D413CB
#define C20EEFC2_0BE4_4918_AAD4_2F0119D413CB
#include <array>
#include <cub.h>
#include <functional>
#include <memory>
#include <set>
#include <string>
#include <zmq.hpp>

#include "msg.h"

CUB_NS_BEGIN

using MsgIdSet    = std::set<msg::mid_t>;
using MsgHandler  = std::function<void( const msg::Header& h_ )>;
using FilterToken = char[ 4 ];

struct Reactor {
    struct Svc {
        Svc( const std::string& endpoint_ );
        ~Svc();

        void init( const std::string& endpoint_ );
        Svc() = default;

        zmq::socket_t  chan;
        zmq::context_t context;
        std::string    endpoint;
    };

    static constexpr int kMaxPubCount = 16;
    static Reactor&      instance();

    ~Reactor();
    Reactor();

    template <typename T>
    int pub( const T& m_ ) {
        return pub( &m_, sizeof( T ) );
    }

    int pub( const void* data_, size_t length_ );
    int sub( const MsgIdSet& msg_set_, MsgHandler h_ );

private:
    zmq::socket_t& distribute( const msg::mid_t& id_ );
    zmq::socket_t& therad_safe_pub();

    int  init();
    void init_svc();

private:
    void filter_from_id( FilterToken& filter_, const msg::mid_t& ids_ );

private:
    Svc _data,
        _trade;
    zmq::context_t _center_ctx;
};

CUB_NS_END

#define REACTOR CUB_NS::Reactor::instance()

#endif /* C20EEFC2_0BE4_4918_AAD4_2F0119D413CB */
