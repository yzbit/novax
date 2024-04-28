#ifndef B6D90468_E9C6_4FE4_A0C4_197D3CC5F83F
#define B6D90468_E9C6_4FE4_A0C4_197D3CC5F83F

#include "definitions.h"
#include "models.h"
#include "ns.h"

NVX_NS_BEGIN

struct ITrader {
    virtual void update( oid_t id_, ostatus_t status_ ) = 0;
    virtual void update( const order_t& o_ )            = 0;
    virtual ~ITrader();
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

struct IData {
    virtual void update( const quotation_t& tick_ ) = 0;
    virtual ~IData();
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

IMarket* create_market( IData* );
IBroker* create_broker( ITrader* );

NVX_NS_END

#endif /* B6D90468_E9C6_4FE4_A0C4_197D3CC5F83F */
