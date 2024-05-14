#ifndef C3C3FFA3_AD73_4439_8415_28F7FB647AAC
#define C3C3FFA3_AD73_4439_8415_28F7FB647AAC
#include "models.h"
#include "ns.h"

NVX_NS_BEGIN

enum class msg_type {
    tick,
    order,
    fund,
    position,
    error
};

struct tick_msg_t {
    msg_type    type = msg_type::tick;
    quotation_t q;
    tick_msg_t() = default;
    tick_msg_t( const quotation_t& q_ )
        : q( q_ ) {}
};

struct fund_msg_t {
    msg_type type = msg_type::fund;
    fund_t   f;
};

union msg_t {
    msg_type   type;
    tick_msg_t tick;
};

struct IPub {
    virtual ~IPub() {}
    virtual int post( const msg_t& m_ ) = 0;
};

#define PUB_MSG( m ) post( ( const msg_t& )m )

NVX_NS_END
#endif /* C3C3FFA3_AD73_4439_8415_28F7FB647AAC */
