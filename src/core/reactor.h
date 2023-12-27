#ifndef CD744141_B668_45C8_AED6_852558124F13
#define CD744141_B668_45C8_AED6_852558124F13

#include <functional>
#include <set>

#include "msg.h"
#include "ns.h"

SATURN_NS_BEGIN

using mid_set_t     = std::set<msg::mid_t>;
using msg_handler_t = std::function<void( const msg::header_t& h_ )>;

struct Reactor {
    virtual ~Reactor() {}

    static Reactor& instance();

    template <typename T>
    int pub( const T& m_ ) {
        return pub( &m_, sizeof( T ) );
    }

    virtual int pub( const void* data_, size_t length_ )           = 0;
    virtual int sub( const mid_set_t& msg_set_, msg_handler_t h_ ) = 0;
};

SATURN_NS_END

#define REACTOR SATURN_NS::Reactor::instance()

#endif /* CD744141_B668_45C8_AED6_852558124F13 */
