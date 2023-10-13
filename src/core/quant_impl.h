#ifndef D392EBBF_A7F9_4ED1_B8F6_BE4B2E02DA07
#define D392EBBF_A7F9_4ED1_B8F6_BE4B2E02DA07

#include "ns.h"
#include "quant.h"

CUB_NS_BEGIN
struct OrderMgmt;
struct Data;
struct Context;
struct Trader;
struct Strategy;

struct QuantImpl : Quant {
    int init() override;
    int execute( Strategy* s_ ) override;

    Trader*    trader();
    Data*      data();
    Strategy*  strategy();
    OrderMgmt* mgmt();

private:
    void quote( const quotation_t& q_ );
    void update( const order_t& o_ );

private:
    void ontick();

private:
    Data*      _d;
    Trader*    _t;
    Strategy*  _s;
    Context*   _c;
    OrderMgmt* _o;

private:
    bool  _working;
    Timer _timer;
};

//--------------INLINES.----------------
inline Data* QuantImpl::data() {
    return _d;
}

inline Trader* QuantImpl::trader() {
    return _t;
}

inline Strategy* QuantImpl::strategy() {
    return _s;
}

inline OrderMgmt* QuantImpl::mgmt() {
    return _o;
}

CUB_NS_END

#endif /* D392EBBF_A7F9_4ED1_B8F6_BE4B2E02DA07 */
