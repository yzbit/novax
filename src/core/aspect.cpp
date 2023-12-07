#include <iostream>

#include "aspect.h"

#include "data.h"
#include "indicator.h"
#include "kline.h"
#include "log.hpp"

CUB_NS_BEGIN

void Aspect::debug() {
    std::cout << "##aspect dump##\n";
    std::cout << "indicators=" << _algos.size() << std::endl;

    for ( auto& i : _algos ) {
        std::cout << "\tname=" << i.i->name() << std::endl;
        std::cout << "\ttracks=" << i.i->tracks() << std::endl;
        std::cout << "\tprio=" << i.p << std::endl;
    }
}

Aspect::~Aspect() {
    delete _k;

    for ( auto& i : _algos ) {
        delete i.i;
    }

    _algos.clear();
}

int Aspect::load( const code_t& code_, const period_t& p_, int count_ ) {
    if ( loaded() ) return 0;

    _symbol = code_;
    _k      = Kline::create( { code_, p_, count_ } );

    CUB_ASSERT( _k );

    return 0;
}

void Aspect::update( const quotation_t& q_ ) {
    _k->on_calc( q_ );

    for ( auto& i : _algos ) {
        i.i->on_calc( *_k, q_ );
    }
}

int Aspect::addi( Indicator* i_ ) {
    if ( !i_ ) return 0;

    auto itr = std::find_if( _algos.begin(), _algos.end(), [ & ]( const prii_t& pi_ ) { return pi_.i == i_; } );

    if ( itr != _algos.end() ) {
        LOG_TAGGED( "asp", "%s already attached. ign", i_->name() );
        return 0;
    }

    _algos.push_back( { _ref_prio++, i_ } );

    _algos.sort( []( auto& a_, auto& b_ ) { return a_.p > b_.p; } );

    return 0;
}

Indicator* Aspect::addi( const string_t& name_, const arg_pack_t& args_ ) {
    auto i = Indicator::create( name_, args_, this );

    if ( !i ) return nullptr;

    addi( i );

    return i;
}

CUB_NS_END
