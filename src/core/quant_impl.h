#ifndef D392EBBF_A7F9_4ED1_B8F6_BE4B2E02DA07
#define D392EBBF_A7F9_4ED1_B8F6_BE4B2E02DA07

#include "ns.h"
#include "quant.h"
#include "timer.h"

CUB_NS_BEGIN
struct OrderMgmt;
struct Data;
struct Context;
struct Trader;
struct Strategy;

struct QuantImpl : Quant {
    QuantImpl();

    Trader*    trader();
    Data*      data();
    Strategy*  strategy();
    OrderMgmt* mgmt();
    Context*   ctx();

    void update( const quotation_t& q_ );
    void invoke();

protected:
    int init();
    int execute( Strategy* s_ ) override;

private:
    void quote( const quotation_t& q_ );
    void update( const order_t& o_ );

private:
    void ontick();

private:
    Data*      _d = nullptr;
    Trader*    _t = nullptr;
    Strategy*  _s = nullptr;
    Context*   _c = nullptr;
    OrderMgmt* _o = nullptr;

private:
    bool  _running = true;
    bool  _working = false;
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

inline Context* QuantImpl::ctx() {
    return _c;
}

CUB_NS_END

#endif /* D392EBBF_A7F9_4ED1_B8F6_BE4B2E02DA07 */
