#ifndef AC98F1C0_3A88_4649_B85A_1852F9A06C67
#define AC98F1C0_3A88_4649_B85A_1852F9A06C67

#include "definitions.h"
#include "models.h"
#include "ns.h"

CUB_NS_BEGIN
struct QuantImpl;
struct Trader {
    Trader( QuantImpl* q_ );

    struct Delegator {
        virtual ~Delegator();
        virtual int start()                  = 0;
        virtual int stop()                   = 0;
        virtual int put( const order_t& o_ ) = 0;
        virtual int cancel( oid_t o_ )       = 0;
    };

    ~Trader();

    int start();
    int stop();
    int put( const order_t& o_ );
    int cancel( oid_t o_ );

private:
    Delegator* _d;
    QuantImpl* _q;
};

CUB_NS_END

#define TRADER Trader::instance()

#endif /* AC98F1C0_3A88_4649_B85A_1852F9A06C67 */
