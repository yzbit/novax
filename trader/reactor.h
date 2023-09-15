#ifndef C20EEFC2_0BE4_4918_AAD4_2F0119D413CB
#define C20EEFC2_0BE4_4918_AAD4_2F0119D413CB
#include "ns.h"

CUB_NS_BEGIN

struct Reactor {
    using MsgIdSet = std::set<msgid_t>;

    static Reactor& instance();

    int pub( int cat_, const Msg& msg_ );
    int sub( const MsgIdSet& msg_set_ );

private:
    int init();
};

CUB_NS_END

#define REACTOR cub::Reactor::instance()

void produce() {
    Msg m;
    REACTOR.pub( 1, m );
}

void consume() {
    REACTOR.sub( { 1, 2, 3 } );

    spwan();
    {
        for ( ;; ) {
            get_msg() {
                process_msg();
            }
        }
    }
}

//那么从理论上来说,这些发布者和订阅者可以在一个机器上也可以在多个机器上,
//但是定于的时候只能从某个确定的reactor上订阅
//如果是一个独立的进程,似乎根本没有必要搞 
void put_order() {
    REACTOR.pub( 1, m );
}

#endif /* C20EEFC2_0BE4_4918_AAD4_2F0119D413CB */
