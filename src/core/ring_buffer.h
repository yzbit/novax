/************************************************************************************
MIT License

Copyright (c) 2024 [YaoZinan zinan@outlook.com nvx-quant.com]

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

*@init: Yaozn
*@contributors:Yaozn
*@update: 2024
**********************************************************************************/

#ifndef CF758CC5_745E_496C_84C3_CAF4B56F5303
#define CF758CC5_745E_496C_84C3_CAF4B56F5303

#include <xmmintrin.h>

#include "ns.h"

NVX_NS_BEGIN

template <typename T, int N>
struct RingBuff {
    int  put( const T& t_ );
    int  pop( T& t_ );
    T*   head();
    int  size();
    void clear();

private:
    volatile int _rd = 0,
                 _wr = 0;
    T _data[ N ];
};

template <typename T, int N>
inline int RingBuff<T, N>::put( const T& t_ ) {
    if ( size() >= N - 1 ) return -1;

    _data[ _wr ] = t_;

    _mm_sfence();
    _wr = ( _wr + 1 ) % N;
    return 0;
}

template <typename T, int N>
inline int RingBuff<T, N>::pop( T& t_ ) {
    if ( size() == 0 ) return -1;

    t_ = _data[ _rd ];

    _mm_lfence();
    _rd = ( _rd + 1 ) % N;

    return 0;
}
template <typename T, int N>
inline void RingBuff<T, N>::clear() {
    _rd = _wr;
}

template <typename T, int N>
inline T* RingBuff<T, N>::head() {
    return size() == 0
               ? nullptr
               : &_data[ _rd ];
}

template <typename T, int N>
inline int RingBuff<T, N>::size() {
    return _wr >= _rd
               ? _wr - _rd
               : _wr + N - _rd;
}

NVX_NS_END
#endif /* CF758CC5_745E_496C_84C3_CAF4B56F5303 */