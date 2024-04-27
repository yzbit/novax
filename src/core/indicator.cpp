#include "indicator.h"

#include "algos/repo.h"
#include "aspect.h"
#include "log.hpp"
#include "series.h"

NVX_NS_BEGIN

/**
假设界面设置了三个参数,分别是int,double,char,如何把这个三个参数按照类型正确的传递

double price;
int count;
char flag;
//--每个指标是知道自己的类型的,他从界面上收集正确的参数类型,然后传给create函数?---显然
//不是....而是要先调用下面的函数,框架如何知道create可变参数的类型
Indicator::create("Ma",count,price,flag);

//用户在界面上选择一个指标Ma,然后按照类型设置了三个参数的值,点确定,框架收集到这些参数,然后传递给create函数,这些参数只能是any,
除非增加traits,告诉我们每个指标的详细
IndiTraits<Rsi>{
    using result=typelist=<int,double,char>;
};





*/
Indicator* Indicator::create( const string_t& name_, const arg_pack_t& args_, Aspect* asp_ ) {
    if ( ALGO.find( name_ ) == ALGO.end() ) {
        LOG_INFO( "cant not find indicator for %s", name_.c_str() );
        return nullptr;
    }

    try {
        Indicator* i = ALGO[ name_ ]( args_ );

        if ( i ) {
            i->set_asp( asp_ );
            asp_->addi( i );
        }

        return i;
    }
    catch ( ... ) {
        return nullptr;  // bad func call
    }
}

Kline& Indicator::ref_base() {
    return _asp->kline();
}

Indicator::~Indicator() {
    for ( auto& [ t, s ] : _series ) {
        delete s;
    }

    _series.clear();
}

Series* Indicator::add_series( int track_, int size_, Series::free_t free_ ) {
    if ( _series.find( track_ ) != _series.end() ) {
        LOG_INFO( "series of track %d already EXISTING", track_ );
    }

    _series.emplace( track_, new Series( size_ ) );

    return _series[ track_ ];
}

Series::element_t* Indicator::value( int track_, int index_ ) {
    auto t = track( track_ );

    return t ? &( t->at( index_ ) ) : nullptr;
}

void Indicator::shift() {
    for ( auto& [ i, s ] : _series ) {
        s->shift();
    }
}

Series::element_t* Indicator::recent() {
    return value();
}

Series* Indicator::track( int index_ ) {
    return _series.find( index_ ) == _series.end()
               ? nullptr
               : _series[ index_ ];
}

NVX_NS_END