#include <memory>

#include "order_mgmt.h"

#include "log.hpp"
#include "quant_impl.h"
#include "trader.h"

CUB_NS_BEGIN

std::atomic<oid_t> OrderMgmt::_init_id = 1;

OrderMgmt::OrderMgmt( MgmtContext* c_ )
    : _c( c_ ) {
}

oid_t OrderMgmt::oid() {
    return ++_init_id;
}

oid_t OrderMgmt::put( const oattr_t& attr_ ) {
    auto ord = std::make_unique<order_t>();
    ord->from_attr( attr_ );
    ord->id = oid();

    auto rc = _c->put_order( *ord.get() );

    if ( rc != 0 )
        return 0;

    _book.emplace( ord->id, ord.release() );

    return ord->id;
}

oid_t OrderMgmt::sellshort( const oattr_t& attr_, price_t sl_, price_t tp_, const text_t& remark = "open short" ) {
    return put( attr_ );
}

oid_t OrderMgmt::buylong( const oattr_t& attr_, price_t sl_, price_t tp_, const text_t& remark_ = "open buy" ) {
    return put( attr_ );
}

int OrderMgmt::cancel( oid_t id_ ) {
    return _c->del_order( id_ );
}

order_t* OrderMgmt::get( oid_t id_ ) {
    if ( _book.find( id_ ) == _book.end() ) {
        LOG_INFO( "can not find order in book: %d, status=%d", id_ );
        return nullptr;
    }

    return _book[ id_ ];
}

//--成交可能是平仓成交，也可能是开仓成交--所以总是要记录下来的--
void OrderMgmt::update( oid_t id_, ostatus_t status_ ) {
    std::unique_lock<std::mutex> lock{ _mutex };

    auto o = get( id_ );

    if ( !o ) return;

    LOG_INFO( "unexpected status %d", status_ );
    CUB_ASSERT( status_ != ostatus_t::dealt );

    if ( ostatus_t::cancelled == status_ ) {
        delete o;
        _book.erase( id_ );
    }
}

void OrderMgmt::create_position( const code_t& code_ ) {
    position_t p     = { 0 };
    p.symbol         = code_;
    instrument_p_t p = { p, p };

    _ins_position.try_emplace( code_, p );
}

position_t* OrderMgmt::position( const code_t& code_, bool long_ ) {
    auto itr_p = _ins_position.find( code_ );

    if ( itr_p == _ins_position.end() ) {
        LOG_INFO( "can not find orde for code = %s,sel = %d, new=%d", code_, long_ );
        return nullptr;
    }

    return &itr_p->second[ long_ ? LONG_POSITION : SHORT_POSITION ];
}

/*
1. 国内四家交易所的平仓顺序统一规则为先开先平。
2. 郑商所在此基础上还有先平单腿持仓，再平组合持仓。
3. 除上期所外的三家交易所在涉及到平今手续费减免时先平今后平昨（后开先平）

//注意因为这里是把仓位合并了，所以和交易软件的能够实时查询不一样，这里只能看到合并后的持仓
*/
void OrderMgmt::herge( order_t* src_, const order_t* update_ ) {
    auto p = position( src_->code,
                       src_->dir == odir_t::sell );

    if ( !p ) {
        LOG_INFO( "no position for order: id=%u, symbol=%s", src_->id, src_->code );
        return;
    }

    vol_t act_closed = std::min( p->qty, update_->qty );

    if ( p->qty <= act_closed )
        LOG_INFO( "over sell/cover of qty:" );

    // if(qty==0) 可以移除了，但是为了计算累计的平仓利润，暂且保留
    //  成交总价值，首先确保p->qty> =0;

    p->qty -= act_closed;
    money_t close_value = update_->price * act_closed;

    p->close_profit += ( src_->dir == odir_t::sell
                             ? update_->price - p->price  // 卖平仓的利润是卖出价格 - 买入价格
                             : p->price - update_->price )
                       * act_closed;

    if ( p->qty == 0 ) {
        p->price = 0;  // else keep
    }

    p->value      = p->price * p->qty;
    p->last_price = update_->price;

    src_->traded += update_->traded;

    if ( src_->traded == src_->qty ) {
        _book.erase( src_->id );
        delete src_;
    }
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

// 所以我们的最好做法是把每个合约的仓位统一成一条，然后算出平均价，每次有成交的时候就简单的处理就好,否则还要区分昨仓，今仓
// 如果这样就会出现合约同时持有long和short，也就是说一个合约应该有两条记录，[0] long汇总 [1]short汇总
void OrderMgmt::accum( order_t* src_, const order_t* update_ ) {
    auto p = position( src_->code,
                       src_->dir == odir_t::p_long );

    if ( !p ) {
        LOG_INFO( "no position for order: id=%u, symbol=%s", src_->id, src_->code );
        return;
    }

    src_->traded += update_->traded;
    p->qty += update_->traded;
    p->price      = ( src_->qty * src_->price + update_->traded * update_->price ) / src_->traded;
    p->last_price = update_->price;

    if ( src_->traded == src_->qty ) {
        _book.erase( src_->id );
        delete src_;
    }
}

//--一个订单可能成交多次
void OrderMgmt::update( const order_t& o_ ) {
    std::unique_lock<std::mutex> lock{ _mutex };

    auto o = get( o_.id );
    if ( !o ) return;

    // todo  最后返回的rtntrade重复的吗，会不会导致数据重算,--如果直接成交了，是不是就没有没有rtnorder，只有一个rtntrade'?不确定需要做测试
    CUB_ASSERT( ostatus_t::dealt == o_.status );

    // o的订单已经成交了o_, 主要的作用是更新持仓
    // o_不仅仅是更新了o，可能还是更新了其他的仓位的，比如平仓的时候
    if ( o_.dir == odir_t::cover || o_.dir == odir_t::sell )
        herge( o, &o_ );
    else
        accum( o, &o_ );
}

int OrderMgmt::sell( const oattr_t& a_ = { "", 0, 0, otype_t::market } ) {
    return close( odir_t::sell, a_ );
}

int OrderMgmt::buy( const oattr_t& a_ = { "", 0, 0, otype_t::market } ) {
    return close( odir_t::cover, a_ );
}
/*如下的仓位如何关闭：
rb2410 long     4
rb2410 short    2

todo 总仓位是 2，那么此时关闭的是什么，关闭净仓? 使用参数决定,我们目前只支持单腿
*/
int OrderMgmt::close( odir_t dir_, const oattr_t& a_ ) {
    CUB_ASSERT( dir_ == odir_t::sell || dir_ == odir_t::cover );

    if ( a_.qty == 0 ) {
        LOG_INFO( "close [%s] with qty=0, and will close all avaiable", a_.symbol );
    }

    auto pv = dir_ == odir_t::sell ? long_position( a_.symbol ) : short_position( a_.symbol );

    if ( 0 >= pv ) {
        LOG_INFO( "no position of [%s] , close ign", a_.symbol );
        return -2;
    }

    if ( pv > a_.qty && a_.qty > 0 )
        pv = a_.qty;

    LOG_INFO( "close position for [%s %d]", a_.symbol, pv );

    order_t order = { 0 };
    order.from_attr( a_ );
    order.qty = pv;
    order.id  = oid();
    order.dir = dir_;

    if ( _c->put_order( order ) != 0 ) {
        LOG_INFO( "close position failed, dir=%d ,sym=%s, qty=%d ,price=%ld ,mode=%d", dir_, a_.symbol, a_.qty, order.price, order.mode );
        return -1;
    }

    _book.emplace( order.code, order );

    return 0;
}

int OrderMgmt::close( oid_t id_ ) {

    return 0;
}

CUB_NS_END