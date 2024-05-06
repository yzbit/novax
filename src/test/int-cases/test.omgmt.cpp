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

#include "../core/dci_role.h"
#include "../core/definitions.h"
#include "../core/log.hpp"
#include "../core/models.h"
#include "../core/order_mgmt.h"
#include "../core/proxy.h"

namespace cub {
struct XContext : cub::MgmtContext {
    int put_order( const order_t& o_ ) {
        return 0;
    }

    int del_order( oid_t id_ ) {
        return 0;
    }
};
}  // namespace cub

int main() {
    LOG_ENABLE_STDOUT();

    auto om = new cub::OrderMgmt( 0 );

    om->start();
    auto ids = om->sellshort( "rb2410", 1, 6543.0, cub::otype_t::market );
    auto idl = om->buylong( "cu2410", 1, 6543.0, cub::otype_t::market );

    om->position( "rb2410" );

    cub::order_t o;
    o.id = ids;
    om->update( o );

    o.id = idl;
    om->update( o );

    om->sell( "rb2410", 1 );
    om->buy( "cu2410", 1 );

    return 0;
}