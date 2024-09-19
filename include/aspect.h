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

#ifndef B51B8BF1_EFFE_4FD1_94C3_9C7FFB93D609
#define B51B8BF1_EFFE_4FD1_94C3_9C7FFB93D609
#include <vector>

#include "definitions.h"
#include "models.h"
#include "ns.h"

NVX_NS_BEGIN

struct kline;
struct data;
struct indicator;
struct aspect final {
    aspect( data* data_ );
    ~aspect();

    void        update( const tick& q_ );
    nvx_st      addi( indicator* i_ );
    kline&      bar( kidx index_ = 0 );
    nvx_st      load( const code& code_, const period& p_, int count_ );
    const code& symbol() const;

private:
    bool loaded() const;
    void debug();

    struct prii_t {
        int        p;
        indicator* i;
    };
    std::vector<prii_t> _algos;

    int    _ref_prio = 1;
    code   _symbol   = "";
    kline* _k        = nullptr;

private:
    data* _data;
};

#if 0
struct asp_repo {
    static AspRepo& instance();
    aspect*         add( const code& code_, const period& p_, int count_ );

private:
    using repo_t = std::vector<aspect>;
    repo_t _repo;
};
#endif

NVX_NS_END

#define ASP AspRepo::instance()

#endif /* B51B8BF1_EFFE_4FD1_94C3_9C7FFB93D609 */
