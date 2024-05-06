/************************************************************************************
The MIT License

Copyright (c) 2024 YaoZinan  [zinan@outlook.com, nvx-quant.com]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

* \author: yaozn(zinan@outlook.com)
* \date: 2024
**********************************************************************************/

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

NVX_NS_BEGIN

#define MA "Ma"
#define KLINE "Kline"

BEGIN_ALGO_REPO()

END_ALGO_REPO()

NVX_NS_END

#define ALGO cub::_s_algo_repo
#endif /* ABC258CE_1A2E_4412_B044_2334BA65204C */
