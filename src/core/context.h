#ifndef C2E26F98_58D2_4FB6_9B05_CB4ED59A65C3
#define C2E26F98_58D2_4FB6_9B05_CB4ED59A65C3
#include "definitions.h"
#include "ns.h"

CUB_NS_BEGIN
struct Indicator;
struct Algo;
struct Aspect;
struct Context {
    Aspect* create( const code_t& symbol_, const period_t& period_, int count_, const string_t& alias_ );
    int     load( const string_t& algo_name_, const ArgPack& arg_ );
    int     load( Algo* a_ );
};

CUB_NS_END

#endif /* C2E26F98_58D2_4FB6_9B05_CB4ED59A65C3 */
