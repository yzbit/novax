#include "pub.h"

#include "concurrentqueue.h"

NVX_NS_BEGIN

struct QuantPub : IPub {
    QuantPub();

private:
    int post( msg_t& m_ ) override;

private:
    static int thread_fun( QuantPub& pub_ );

private:
    int a;
};

NVX_NS_END