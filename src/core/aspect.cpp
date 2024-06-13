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

#include <iostream>

#include "aspect.h"

#include "data.h"
#include "indicator.h"
#include "kline.h"
#include "log.hpp"

NVX_NS_BEGIN

void aspect::debug() {
    std::cout << "##aspect dump##\n";
    std::cout << "indicators=" << _algos.size() << std::endl;

    for ( auto& i : _algos ) {
        std::cout << "\tname=" << i.i->name() << std::endl;
        std::cout << "\ttrakcs=" << i.i->nvalue() << std::endl;
        std::cout << "\tprio=" << i.p << std::endl;
    }
}

kline& aspect::bar( kidx index_ ) {
    return *_k;
}

const code& aspect::symbol() const {
    return _symbol;
}

inline bool aspect::loaded() const {
    return !_symbol.empty();
}

aspect::~aspect() {
    delete _k;

    for ( auto& i : _algos ) {
        delete i.i;
    }

    _algos.clear();
}

nvx_st aspect::load( const code& code_, const period& p_, int count_ ) {
    if ( loaded() ) return 0;

    _symbol = code_;
    _k      = new kline( code_, p_, count_ );

    NVX_ASSERT( _k );

    return NVX_OK;
}

void aspect::update( const tick& q_ ) {
    _k->update( q_ );

    for ( auto& i : _algos ) {
        i.i->calc( q_, *_k );
    }
}

nvx_st aspect::addi( indicator* i_ ) {
    if ( !i_ ) return NVX_OK;

    auto itr = std::find_if( _algos.begin(), _algos.end(), [ & ]( const prii_t& pi_ ) { return pi_.i == i_; } );

    if ( itr != _algos.end() ) {
        LOG_TAGGED( "asp", "%s already attached. ign", i_->name() );
        return NVX_OK;
    }

    _algos.push_back( { _ref_prio++, i_ } );
    std::sort( _algos.begin(), _algos.end(), []( auto& a_, auto& b_ ) { return a_.p > b_.p; } );

    return NVX_OK;
}

aspect::aspect( data* data_ )
    : _data( data_ ) {}

NVX_NS_END
