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

#ifndef A37995CF_4ECF_464C_B471_04E26CC7055C
#define A37995CF_4ECF_464C_B471_04E26CC7055C
#include <functional>
#include <stdexcept>
#include <string.h>
#include <type_traits>

#include "ns.h"

NVX_NS_BEGIN

/**
 * for各种计算,当前正在变化的元素flux也要计数,比如已经收集了3根x线,加上flux是4根
 * ma(4)可以计算。
 * size 应该返回包括flux的元素个数
 * kindex , from right to left: 0 1 2 3...count-1
 */
template <typename T>
struct series final {
    using element_t = T;
    using visitor_t = std::function<bool( element_t& )>;

    series( int n_ );
    ~series();

    size_t size();
    void   for_each( visitor_t v_ );
    void   for_each_r( visitor_t v_ );
    void   update( kidx index_, const element_t& t_ );

    element_t* append( const element_t& t_ );
    element_t* flux();
    element_t* advance();
    element_t* operator[]( kidx index_ );
    element_t* get( kidx index_ );
    element_t* at( kidx index_ );  // 0 is flux, not stable

private:
    size_t i2s( kidx index_ );

private:
    size_t _total = 0;
    size_t _ptr   = 0;
    size_t _count = 0;  // all elements including flux, max(_count) = _total

private:
    element_t* _values;
};

template <typename T>
inline series<T>::series( int n_ ) {
    _total = n_;
    _count = 0;

    static_assert( std::is_copy_assignable_v<T>, "bad series type" );
    _values = new element_t[ _total ]{ 0 };
}

template <typename T>
inline series<T>::~series() {
    delete[] _values;
}

template <typename T>
inline typename series<T>::element_t* series<T>::advance() {
    if ( 0 == _count ) {
        _count = 1;
        _ptr   = 0;
    }
    else {
        _ptr   = ( _ptr + 1 ) % _total;
        _count = _count < _total ? _count + 1
                                 : _count;
    }

    memset( flux(), 0x00, sizeof( T ) );
    return flux();
}

template <typename T>
inline typename series<T>::element_t* series<T>::flux() {
    if ( _count == 0 )
        return nullptr;

    return get( 0 );
}

template <typename T>
inline typename series<T>::element_t* series<T>::append( const element_t& t_ ) {
    auto* e = advance();
    *e      = t_;
    return e;
}

template <typename T>
inline size_t series<T>::size() {
    return _count;
}

template <typename T>
inline void series<T>::for_each( typename series<T>::visitor_t v_ ) {
    for ( auto loop = 0; loop < _count; ++loop ) {
        if ( !v_( get( loop ) ) )
            break;
    }
}

template <typename T>
inline void series<T>::for_each_r( typename series<T>::visitor_t v_ ) {
    if ( _count == 0 )
        return;

    for ( auto loop = _count - 1; loop >= 0; --loop ) {
        if ( !v_( get( loop ) ) )
            break;
    }
}

template <typename T>
inline void series<T>::update( kidx index_, const element_t& t_ ) {
    get( index_ ) = t_;
}

//* index_= 0 points to ptr
template <typename T>
inline size_t series<T>::i2s( kidx index_ ) {
    assert( index_ < _count );

    return _ptr >= index_ ? _ptr - index_
                          : _ptr - index_ + _total;
}

template <typename T>
inline typename series<T>::element_t* series<T>::operator[]( kidx index_ ) {
    return get( index_ );
}

template <typename T>
inline typename series<T>::element_t* series<T>::at( kidx index_ ) {
    return get( index_ );
}

template <typename T>
inline typename series<T>::element_t* series<T>::get( kidx index_ ) {
    if ( index_ >= _count ) {  //! (index_0 , _count = 0) => nil
        return nullptr;
    }

    return &_values[ i2s( index_ ) ];
}

NVX_NS_END

#endif /* A37995CF_4ECF_464C_B471_04E26CC7055C */
