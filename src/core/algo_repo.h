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

#define DECL_ALGO( _name_, _creator_ ) {##_name_, _creator_ },

CUB_NS_BEGIN

BEGIN_ALGO_REPO()

DECL_ALGO( "ma", Ma::create )
DECL_ALGO( "kline", Kline::create )

END_ALGO_REPO()

CUB_NS_END

#define ALGO cub::_s_algo_repo
#endif /* B124668D_7ECD_44F4_8B97_03EC927050F9 */
