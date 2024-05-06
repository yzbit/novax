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

#include <memory>

#include "order_mgmt.h"

#include "context.h"
#include "ctp/ctp_trade_proxy.h"
#include "log.hpp"
#include "proxy.h"
#include "quant.h"

NVX_NS_BEGIN

IBroker* create_broker( ITrader* );

std::atomic<oid_t> OrderMgmt::_init_id = 1;

OrderMgmt::~OrderMgmt() {
}

OrderMgmt::OrderMgmt() {
}

oid_t OrderMgmt::oid() {
    return ++_init_id;
}

int OrderMgmt::start() {
    return ib()->start();
}

int OrderMgmt::stop() {
    return ib()->stop();
}

oid_t OrderMgmt::put( const odir_t& dir_,
                      const code_t& code_,
                      const vol_t   qty_,
                      const price_t price_,
                      const otype_t mode_,
                      const price_t sl_,
                      const price_t tp_,
                      const text_t& remark_ ) {

    auto r   = order_t( code_, qty_, price_, mode_, dir_ );
    r.id     = oid();
    r.remark = remark_;

    _book.emplace( r.id, r );  // impt
    if ( 0 != ib()->put( r ) ) {
        LOG_INFO( "put order failed, delete it;oid=[%u]", r.id );
        _book.erase( r.id );
        return kBadId;
    }

    LOG_INFO( "order count in book: [ %d ]", _book.size() );
    return r.id;
}

int OrderMgmt::close( const code_t code_ ) {
    NVX_ASSERT( 0 );

    return 0;
}

oid_t OrderMgmt::sellshort( const code_t& code_,
                            const vol_t   qty_,
                            const price_t price_,
                            const otype_t mode_,
                            const price_t sl_,
                            const price_t tp_,
                            const text_t& remark_ ) {
    LOG_TAGGED( "om", "short: code=%s qty%d price=%.2f sl=%d, tp=%d, r=%s", code_.c_str(), qty_, price_, ( int )mode_, sl_, tp_, remark_.c_str() );
    return put( odir_t::p_short, code_, qty_, price_, mode_, sl_, tp_, remark_ );
}

oid_t OrderMgmt::buylong( const code_t& code_,
                          const vol_t   qty_,
                          const price_t price_,
                          const otype_t mode_,
                          const price_t sl_,
                          const price_t tp_,
                          const text_t& remark_ ) {
    LOG_TAGGED( "om", "long: code=%s qty%d price=%.2f sl=%d, tp=%d, r=%s", code_.c_str(), qty_, price_, ( int )mode_, sl_, tp_, remark_.c_str() );
    return put( odir_t::p_long, code_, qty_, price_, mode_, sl_, tp_, remark_ );
}

int OrderMgmt::cancel( oid_t id_ ) {
    LOG_TAGGED( "om", "del order: %u", id_ );
    auto o = get( id_ );
    if ( !o ) {
        LOG_TAGGED( "om", "cannot find order: %u", id_ );
        return -1;
    }

    auto& r = o.value().get();
    if ( !o
         || ( r.status != ostatus_t::pending
              && r.status != ostatus_t::partial_dealed
              && r.status != ostatus_t::patial_canelled ) ) {
        LOG_TAGGED( "om", "can not cancel order, id=%u status=%d", id_, r.status );
        return -1;
    }

    return ib()->cancel( r );
}

OrderMgmt::OrderOpt OrderMgmt::get( oid_t id_ ) {
    if ( _book.find( id_ ) == _book.end() ) {
        LOG_INFO( "can not find order in book: %d, status=%d", id_ );
        return std::nullopt;
    }

    return _book[ id_ ];
}

//--成交可能是平仓成交，也可能是开仓成交--所以总是要记录下来的--
void OrderMgmt::update_ord( oid_t id_, ostatus_t status_ ) {
    std::unique_lock<std::mutex> lock{ _mutex };

    auto o = get( id_ );

    if ( !o ) return;

    LOG_INFO( "unexpected status %d", status_ );
    NVX_ASSERT( status_ != ostatus_t::dealt );

    // todo 会不会出现部分canclled
    if ( ostatus_t::cancelled == status_ ) {
        // 无论是部分撤掉(部分已经成交) 还是全部撤掉, 撤单只能是基于整个订单的
        LOG_INFO( "close order: id=%u ", id_ );
        _book.erase( id_ );
    }
}

position_t* OrderMgmt::position( const code_t& code_, bool long_ ) {
    auto itr_p = _ins_position.find( code_ );

    if ( itr_p == _ins_position.end() ) {
        LOG_INFO( "new position; can not find orde for code = %s,sel = %d", code_.c_str(), long_ );

        position_t p{ 0 };
        p.symbol          = code_;
        portfilio_t ins_p = { p, p };

        _ins_position.try_emplace( code_, ins_p );
    }

    return &itr_p->second[ long_ ? LONG_POSITION : SHORT_POSITION ];
}

/*
1. 国内四家交易所的平仓顺序统一规则为先开先平。
2. 郑商所在此基础上还有先平单腿持仓，再平组合持仓。
3. 除上期所外的三家交易所在涉及到平今手续费减免时先平今后平昨（后开先平）

//注意因为这里是把仓位合并了，所以和交易软件的能够实时查询不一样，这里只能看到合并后的持仓
*/
void OrderMgmt::herge( order_t& src_, const order_t& update_ ) {
    auto p = position( src_.code, src_.dir == odir_t::sell );  //-sell -> p_long

    vol_t act_closed = std::min( p->qty, update_.qty );

    if ( p->qty <= act_closed )
        LOG_INFO( "over sell/cover of qty:" );

    // if(qty==0) 可以移除了，但是为了计算累计的平仓利润，暂且保留
    //  成交总价值，首先确保p->qty> =0;

    p->qty -= act_closed;
    // todo
    [[maybe_unused]] money_t close_value = update_.price * act_closed;

    p->close_profit += ( src_.dir == odir_t::sell
                             ? update_.price - p->price  // 卖平仓的利润是卖出价格 - 买入价格
                             : p->price - update_.price )
                       * act_closed;

    if ( p->qty == 0 ) {
        LOG_INFO( "position closed: %s", p->symbol.c_str() );
        p->price = 0;  // else keep
    }

    p->value      = p->price * p->qty;
    p->last_price = update_.price;
}

vol_t OrderMgmt::position( const code_t& code_ ) {
    return long_position( code_ ) - short_position( code_ );
}

vol_t OrderMgmt::short_position( const code_t& code_ ) {
    if ( auto p = position( code_, false ); p ) {
        return p->qty;
    }

    return 0;
}

vol_t OrderMgmt::long_position( const code_t& code_ ) {
    if ( auto p = position( code_, true ); p ) {
        return p->qty;
    }

    return 0;
}

void OrderMgmt::update_fund( const fund_t& f_ ) {
    QUANT.context()->update_fund( f_ );
}

void OrderMgmt::update_position() {
}
// 所以我们的最好做法是把每个合约的仓位统一成一条，然后算出平均价，每次有成交的时候就简单的处理就好,否则还要区分昨仓，今仓
// 如果这样就会出现合约同时持有long和short，也就是说一个合约应该有两条记录，[0] long汇总 [1]short汇总
void OrderMgmt::accum( order_t& src_, const order_t& update_ ) {
    auto p = position( src_.code, src_.dir == odir_t::p_long );

    p->qty += update_.traded;
    p->price      = ( src_.qty * src_.price + update_.traded * update_.price ) / p->qty;
    p->last_price = update_.price;
}

//--一个订单可能成交多次
void OrderMgmt::update_ord( const order_t& o_ ) {
    std::unique_lock<std::mutex> lock{ _mutex };

    LOG_INFO( "update order id=%u, status=[ %d,0x%x ], dir=%d", o_.id, o_.status, o_.status, o_.dir );
    auto o = get( o_.id );

    if ( !o ) {
        LOG_INFO( "cannot find order of id=%u", o_.id );
        return;
    }
    auto& r = o.value().get();

    // todo  最后返回的rtntrade重复的吗，会不会导致数据重算,--如果直接成交了，是不是就没有没有rtnorder，只有一个rtntrade'?不确定需要做测试
    NVX_ASSERT( ostatus_t::dealt == o_.status );

    // o的订单已经成交了o_, 主要的作用是更新持仓
    // o_不仅仅是更新了o，可能还是更新了其他的仓位的，比如平仓的时候
    if ( o_.dir == odir_t::cover || o_.dir == odir_t::sell )
        herge( r, o_ );
    else
        accum( r, o_ );

    r.traded += o_.traded;

    if ( r.traded == r.qty ) {
        LOG_INFO( "order of [ %u ] complete dealt, eraise it", r.id );
        _book.erase( r.id );
    }
}

int OrderMgmt::sell( const code_t& code_,
                     const vol_t   qty_,
                     const price_t price_,
                     const otype_t mode_,
                     const text_t& remark_ ) {
    LOG_TAGGED( "om", "sell: code=%s qty=%d price=%.2f, mode=%d, r=%s", code_.c_str(), qty_, price_, ( int )mode_, remark_.c_str() );

    order_t r( code_, qty_, price_, mode_, odir_t::sell );
    r.remark = remark_;
    r.id     = oid();

    return close( r );
}

int OrderMgmt::buy( const code_t& code_,
                    const vol_t   qty_,
                    const price_t price_,
                    const otype_t mode_,
                    const text_t& remark_ ) {
    LOG_TAGGED( "om", "buy: code=%s qty=%d price=%.2f, mode=%d, r=%s", code_.c_str(), qty_, price_, ( int )mode_, remark_.c_str() );
    order_t r( code_, qty_, price_, mode_, odir_t::cover );
    r.id = oid();

    return close( r );
}

/*如下的仓位如何关闭：
rb2410 long     4
rb2410 short    2

todo 总仓位是 2，那么此时关闭的是什么，关闭净仓? 使用参数决定,我们目前只支持单腿
*/
int OrderMgmt::close( const order_t& r_ ) {
    NVX_ASSERT( r_.dir == odir_t::sell || r_.dir == odir_t::cover );

    if ( r_.qty == 0 ) {
        LOG_INFO( "close [%s] with qty=0, !!will close all avaiable", r_.code.c_str() );
    }

    auto pv = r_.dir == odir_t::sell
                  ? long_position( r_.code.c_str() )
                  : short_position( r_.code.c_str() );

    if ( 0 >= pv ) {
        LOG_INFO( "no position of [%s] , close ign", r_.code.c_str() );
        return -2;
    }

    if ( pv > r_.qty && r_.qty > 0 ) pv = r_.qty;

    LOG_INFO( "close position for [%s %d]", r_.code.c_str(), pv );

    if ( 0 != ib()->put( r_ ) ) {
        LOG_INFO( "close position failed,id=%u ,sym=%s", r_.id, r_.code.c_str() );
        return -1;
    }

    // todo
    //_book.emplace( order.code, order );

    return 0;
}

int OrderMgmt::close( oid_t id_ ) {

    return 0;
}

NVX_NS_END