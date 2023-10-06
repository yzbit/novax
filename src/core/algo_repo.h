#ifndef B124668D_7ECD_44F4_8B97_03EC927050F9
#define B124668D_7ECD_44F4_8B97_03EC927050F9
#include <map>

#include "algos/all.h"
#include "definitions.h"
#include "ns.h"

#define BEGIN_ALGO_REPO() static std::map<string_t, algo_creator_t> _s_algo_repo = {

#define END_ALGO_REPO() \
    }                   \
    ;

// #define DECL_ALGO( _name_, _creator_ ) { #_name_, _creator_ },

#define __NAME__( _n_ ) #_n_
#define DECL_ALGO( _name_ ) { __NAME__( _name_ ), _name_::create },

#define ALGO_NAME( _t_ ) __NAME__( _t_ )

CUB_NS_BEGIN

#define MA Ma
#define KLINE Kline

BEGIN_ALGO_REPO()

// DECL_ALGO( ALGO_MA, Ma::create )
// DECL_ALGO( ALGO_KLINE, Kline::create )
DECL_ALGO( KLINE )

END_ALGO_REPO()

CUB_NS_END

#define ALGO cub::_s_algo_repo
#endif /* B124668D_7ECD_44F4_8B97_03EC927050F9 */
