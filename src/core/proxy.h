#ifndef B6D90468_E9C6_4FE4_A0C4_197D3CC5F83F
#define B6D90468_E9C6_4FE4_A0C4_197D3CC5F83F

#include "definitions.h"
#include "models.h"
#include "ns.h"

NVX_NS_BEGIN

struct IBroker;
struct ITrader {
    virtual void update_ord( oid_t id_, ostatus_t status_ ) = 0;
    virtual void update_ord( const order_t& o_ )            = 0;
    virtual void update_fund( const fund_t& f_ )            = 0;
    virtual void update_position()                          = 0;
    virtual ~ITrader();

protected:
    IBroker* ib() { return _ib; }

private:
    IBroker* _ib;
    friend IBroker;
};

struct IBroker {
    IBroker( ITrader* tr_ );
    virtual ~IBroker();

    virtual int start()                     = 0;
    virtual int stop()                      = 0;
    virtual int put( const order_t& o_ )    = 0;
    virtual int cancel( const order_t& o_ ) = 0;

protected:
    ITrader* delegator();

private:
    ITrader* _tr = nullptr;
};

struct IMarket;
struct IData {
    virtual void update( const quotation_t& tick_ ) = 0;
    virtual ~IData();

protected:
    IMarket* market() { return _m; }

private:
    IMarket* _m;
    friend IMarket;
};

struct IMarket {
    IMarket( IData* dt_ );

    virtual int start()                            = 0;
    virtual int stop()                             = 0;
    virtual int subscribe( const code_t& code_ )   = 0;
    virtual int unsubscribe( const code_t& code_ ) = 0;

    virtual ~IMarket();

protected:
    IData* delegator();

private:
    IData* _dt = nullptr;
};

NVX_NS_END

#endif /* B6D90468_E9C6_4FE4_A0C4_197D3CC5F83F */
