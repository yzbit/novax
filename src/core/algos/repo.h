#ifndef ABC258CE_1A2E_4412_B044_2334BA65204C
#define ABC258CE_1A2E_4412_B044_2334BA65204C

#include <map>

#include "../definitions.h"
#include "../ns.h"
#include "all.h"

#define BEGIN_ALGO_REPO() static std::map<string_t, algo_creator_t> _s_algo_repo = {

#define END_ALGO_REPO() \
    }                   \
    ;

// #define DECL_ALGO( _name_, _creator_ ) { #_name_, _creator_ },

#define __NAME__( _n_ ) #_n_
#define DECL_ALGO( _name_ ) { __NAME__( _name_ ), _name_::create },

#define ALGO_NAME( _t_ ) __NAME__( _t_ )

SATURN_NS_BEGIN

#define MA "Ma"
#define KLINE "Kline"

BEGIN_ALGO_REPO()

END_ALGO_REPO()

SATURN_NS_END

#define ALGO cub::_s_algo_repo
#endif /* ABC258CE_1A2E_4412_B044_2334BA65204C */
