#ifndef CB707429_D0BF_4054_8203_04C2541044AD
#define CB707429_D0BF_4054_8203_04C2541044AD
#include <memory>
#include <ns.h>
#include <zmq.hpp>

CUB_NS_BEGIN

template <typename T>
struct Service : T {
    void on_msg_received() {
        T::on_msg_received();
    }

    Service( zmq::context_t& context_ );

    void        init();
    const char* rt_address() {
        return T::rt_address();
    }

    const char* ctl_address() {
        return T::ctl_address();
    }

public:
    std::unique_ptr<zmq::socket_t> _rt_channel;
    std::unique_ptr<zmq::socket_t> _ctl_channel;
};

template <typename T>
inline Service<T>::Service( zmq::context_t& context_ ) {
    _rt_channel  = std::make_unique<zmq::socket_t>( context_, zmq::socket_type::pub );
    _ctl_channel = std::make_unique<zmq::socket_t>( context_, zmq::socket_type::rep );

    _rt_channel->bind( rt_address );
    _ctl_channel->bind( ctl_address );
}

template <typename T>
inline void Service<T>::init() {
}

CUB_NS_END

#endif /* CB707429_D0BF_4054_8203_04C2541044AD */
