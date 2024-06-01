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

#ifndef A0120CD0_E1AB_40A0_93BC_9BE6188CDA2A
#define A0120CD0_E1AB_40A0_93BC_9BE6188CDA2A

#include <algorithm>
#include <vector>

#include "definitions.h"
#include "models.h"
#include "ns.h"
#include "positionimpl.h"
#include "proxy.h"

// order mgmt  只管理订单和仓位，其他的不负责，甚至订单都可以不用管理
NVX_NS_BEGIN
struct position;
struct broker;

struct portfolio {
    enum class dist_t : char {
        netlong,
        netshort
    };

    using PosRepo = std::vector<position_impl*>;

    portfolio();

    nvx_st         update( const order_update& upt_ );
    position_impl* pos( const code& code_, dist_t d_ );
    position_impl* add( const code& code_, dist_t d_ );

private:
    PosRepo _repo;
};

// todo. sync to disk.
struct ord_book {
    ord_book();
    ~ord_book();

    size_t count() const;
    // oid    gen_id();
    order* find( oid id_ );
    order* append( order& order_ );

private:
    void persist() {
        // todo
    }

private:
    std::unordered_map<oid, order> _ords;

    oid _start_id;
};

//--------------------------
struct order_mgmt {
    order_mgmt( broker* ib_ );
    ~order_mgmt();

    nvx_st start();
    nvx_st stop();

    //-price=0 use market
    oid buylong( const code& code_,
                 vol         qty_,
                 price       price_,
                 ord_type    mode_,
                 const text& remark );
    oid sell( const code& code_,
              vol         qty_,
              price       price_,
              ord_type    mode_,
              const text& remark );
    oid sellshort( const code& code_,
                   vol         qty_,
                   price       price_,
                   ord_type    mode_,
                   const text& remark );
    oid buy( const code& code_,
             vol         qty_,
             price       price_,
             ord_type    mode_,
             const text& remark );

    oid cancel( oid id_ );
    oid close( const code& code_ );

    position* pos_of( const code& code_, bool long_ );
    void      update_ord( const order_update& ord_ );
    void      update_position();

private:
    // oid close( position* p_, vol qty_ );
    oid put( ord_dir     dir_,
             const code& code_,
             vol         qty_,
             price       price_,
             ord_type    mode_,
             const text& remark_ );
    // nvx_st remove( oid id );

private:
    portfolio _pf;
    ord_book  _orders;

private:
    broker* _ib;
};

NVX_NS_END

#endif /* A0120CD0_E1AB_40A0_93BC_9BE6188CDA2A */
