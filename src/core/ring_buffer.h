#ifndef CF758CC5_745E_496C_84C3_CAF4B56F5303
#define CF758CC5_745E_496C_84C3_CAF4B56F5303

#include <xmmintrin.h>

#include "ns.h"

CUB_NS_BEGIN

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

CUB_NS_END
#endif /* CF758CC5_745E_496C_84C3_CAF4B56F5303 */