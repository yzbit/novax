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

template <typename T>
struct series final {
    using element_t = T;
    using visitor_t = std::function<bool( element_t& )>;

    series( int n_ );
    ~series();

    size_t size();
    void   for_each( visitor_t v_ );
    void   update( kidx index_, const element_t& t_ );

    element_t& append( const element_t& t_ );
    element_t& current();
    element_t& shift();
    element_t& operator[]( kidx index_ );
    element_t& get( kidx index_ );
    element_t& at( kidx index_ );

private:
    size_t idx2slot( kidx index_ );
    void   inc();

private:
    size_t _total = 0;
    size_t _begin = 0;
    size_t _end   = 0;
    size_t _size  = 0;

private:
    element_t* _values;
};

#define SERIES_ROUND( begin, end, total ) \
    do {                                  \
        if ( end == begin ) ++begin;      \
        if ( end == total ) end = 0;      \
        if ( begin == total ) begin = 0;  \
    } while ( 0 )

template <typename T>
inline series<T>::series( int n_ ) {
    _total = n_;
    _size  = 1;

    static_assert( std::is_copy_assignable_v<T>, "bad series type" );
    _values = new element_t[ n_ ]{ 0 };
}

template <typename T>
inline void series<T>::inc() {
    if ( _size < _total ) {
        ++_size;
    }
}

template <typename T>
inline series<T>::~series() {
    delete[] _values;
}

template <typename T>
inline typename series<T>::element_t& series<T>::shift() {
    ++_end;
    SERIES_ROUND( _begin, _end, _total );
    inc();
    return current();
}

template <typename T>
inline typename series<T>::element_t& series<T>::current() {
    return get( 0 );
}

template <typename T>
inline typename series<T>::element_t& series<T>::append( const element_t& t_ ) {
    auto& e = shift();
    e       = t_;
    return e;
}

template <typename T>
inline size_t series<T>::size() {
    return _size;
}

template <typename T>
inline void series<T>::for_each( typename series<T>::visitor_t v_ ) {
    size_t n = size();
    for ( int loop = _begin; n--; loop = ( loop + 1 ) % _total ) {
        if ( !v_( _values[ loop ] ) ) break;
    }
}

template <typename T>
inline void series<T>::update( kidx index_, const element_t& t_ ) {
    get( index_ ) = t_;
}

template <typename T>
inline size_t series<T>::idx2slot( kidx index_ ) {
    // 0-~n-1
    if ( index_ >= size() ) {
        throw std::range_error( "series index overflow" );
    }

    return ( _end - index_ + _total ) % _total;
}

template <typename T>
inline typename series<T>::element_t& series<T>::operator[]( kidx index_ ) {
    return get( index_ );
}

template <typename T>
inline typename series<T>::element_t& series<T>::at( kidx index_ ) {
    return get( index_ );
}

template <typename T>
inline typename series<T>::element_t& series<T>::get( kidx index_ ) {
    return _values[ idx2slot( index_ ) ];
}

NVX_NS_END

#endif /* A37995CF_4ECF_464C_B471_04E26CC7055C */
