#ifndef BB30CC9E_0827_4895_93A9_D7B8DC7EA5FB
#define BB30CC9E_0827_4895_93A9_D7B8DC7EA5FB
#include "../ns.h"
#include "../strategy.h"

USE_CUB_NS

struct BreakTh : Strategy {
    BreakTh( const code_t& code_ )
        : _code( code_ ) {}

    void on_refresh( Context& c );
    void on_init( Context& c );

private:
    code_t     _code;
    Indicator* _ma = nullptr;
};

#endif /* BB30CC9E_0827_4895_93A9_D7B8DC7EA5FB */
