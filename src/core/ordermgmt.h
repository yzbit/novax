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
struct IPosition;
struct IBroker;

struct Portfolio {
    enum class dist_t : char {
        netlong,
        netshort
    };

    using PosRepo = std::vector<PositionImpl*>;

    Portfolio();

    nvx_st        update( const order_update_t& upt_ );
    PositionImpl* pos( const code_t& code_, dist_t d_ );
    PositionImpl* add( const code_t& code_, dist_t d_ );

private:
    PosRepo _repo;
};

// todo. sync to disk.
struct OrderBook {
    OrderBook( oid_t init_id_ );
    ~OrderBook();

    size_t   count() const;
    oid_t    oid();
    order_t* find( oid_t id_ );
    order_t* append( order_t& order_ );

private:
    void persist() {
        // todo
    }

private:
    std::unordered_map<oid_t, order_t> _ords;

    oid_t _start_id;
};

//--------------------------
struct OrderMgmt {
    OrderMgmt( IBroker* ib_, oid_t id_start_ );
    ~OrderMgmt();

    nvx_st start();
    nvx_st stop();

    //-price=0 use market
    oid_t  buylong( const code_t& code_,
                    vol_t         qty_,
                    price_t       price_,
                    otype_t       mode_,
                    const text_t& remark );
    nvx_st sell( const code_t& code_,
                 vol_t         qty_,
                 price_t       price_,
                 otype_t       mode_,
                 const text_t& remark );
    oid_t  sellshort( const code_t& code_,
                      vol_t         qty_,
                      price_t       price_,
                      otype_t       mode_,
                      const text_t& remark );
    nvx_st buy( const code_t& code_,
                vol_t         qty_,
                price_t       price_,
                otype_t       mode_,
                const text_t& remark );

    // 这里不能用oid好像，因为order id最终会转化为position,当然因为我们其实是记得自己的订单有多少转为了position的
    // 可以先只支持单腿操作，也就是如果既有空单又有多单，那么选择合适的
    // 有没有可能一个价格同时是多单和空单的止损单，多单价格p1，空单价格p0，多单的止损必须是小于p1，空单的止损是大于p0，只要p1>p>p0，就成立, 此时你很难讲p是给谁做止损的
    nvx_st stop( const code_t& code_, vol_t qty_, price_t price_ );
    nvx_st profit( const code_t& code_, vol_t qty_, price_t price_ );
    nvx_st cancel( oid_t id_ );
    nvx_st close( oid_t id_ );
    nvx_st close( const code_t& code_ );

    IPosition* position( const code_t& code_, bool long_ );
    void       update_ord( const order_update_t& ord_ );
    void       update_position();

    // >0 表示long多余short
    vol_t position( const code_t& code_ );
    vol_t short_position( const code_t& code_ );
    vol_t long_position( const code_t& code_ );

private:
    nvx_st close( const order_t& r_ );
    oid_t  put( odir_t        dir_,
                const code_t& code_,
                vol_t         qty_,
                price_t       price_,
                otype_t       mode_,
                const text_t& remark_ );
    nvx_st remove( oid_t id );

private:
    Portfolio _pf;
    OrderBook _record;

private:
    IBroker* _ib;
};

NVX_NS_END

#endif /* A0120CD0_E1AB_40A0_93BC_9BE6188CDA2A */
