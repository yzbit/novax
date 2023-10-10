#ifndef C20EEFC2_0BE4_4918_AAD4_2F0119D413CB
#define C20EEFC2_0BE4_4918_AAD4_2F0119D413CB
#include <array>
#include <functional>
#include <memory>
#include <set>
#include <zmq.hpp>

#include "definitions.h"
#include "msg.h"
#include "ns.h"
#include "reactor.h"

CUB_NS_BEGIN

using FilterToken = char[ 4 ];

struct ReactorImpl : Reactor {
    static constexpr int kMaxPubCount = 16;

    ~ReactorImpl();
    ReactorImpl();

    int pub( const void* data_, size_t length_ ) override;
    int sub( const mid_set_t& msg_set_, msg_handler_t h_ ) override;

private:
    zmq::socket_t& distribute( const msg::mid_t& id_ );
    zmq::socket_t& therad_safe_pub();

    int init();

private:
    void filter_from_id( FilterToken& filter_, const msg::mid_t& ids_ );

private:
    zmq::context_t _center_ctx;
};

CUB_NS_END

#endif /* C20EEFC2_0BE4_4918_AAD4_2F0119D413CB */
