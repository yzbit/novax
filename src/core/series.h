#ifndef A37995CF_4ECF_464C_B471_04E26CC7055C
#define A37995CF_4ECF_464C_B471_04E26CC7055C
#include <functional>
#include <stdexcept>
#include <string.h>
#include <type_traits>

#include "ns.h"

NVX_NS_BEGIN

template <typename T>
struct Series final {
    using element_t = T;
    using visitor_t = std::function<bool( element_t& )>;

    Series( int n_ );
    ~Series();

    //  void append( const element_t& t_ );
    void shift();
    int  size();
    void for_each( visitor_t& v_ );
    void update( int index_, const element_t& t_ );

    element_t& operator[]( int index_ );
    element_t& get( int index_ );
    element_t& at( int index_ );

private:
    bool is_valid_slot( int slot_ );

    // 0 是最右边的那个,也就是_end--把K线序列映射为存储序列
    int map( int index_ );

private:
    int        _total;
    int        _begin = 0;
    int        _end   = 0;
    element_t* _values;
};

#define NVX_ROUND( _index_, _capa_sz_ ) \
    _index_ = _index_ == _capa_sz_      \
                  ? 0                   \
                  : _index_

template <typename T>
inline Series<T>::Series( int n_ ) {
    _total = n_ + 1;
    static_assert( std::is_copy_assignable_v<T>, "bad series type" );

    _values = new element_t[ n_ ]{ 0 };
    // memcpy( _values, 0x00, n_ * sizeof( element_t ) );
    shift();
}

template <typename T>
inline Series<T>::~Series() {
    delete[] _values;
    _values = nullptr;
}

template <typename T>
inline void Series<T>::shift() {
    ++_end;
    NVX_ROUND( _end, _total );

    if ( _end == _begin ) {
        ++_begin;
        NVX_ROUND( _begin, _total );
    }
}

/*
inline void Series::append( const element_t& t_ ) {
    _values[ _end ] = t_;

    ++_end;
    ROUND_PTR( _end, _total );

    if ( _end == _begin ) {
        _free( _values[ _begin ].p );

        ++_begin;
        ROUND_PTR( _begin, _total );
    }
}
*/

template <typename T>
inline int Series<T>::size() {
    return _end >= _begin
               ? _end - _begin
               : _end + _total - _begin;  //-减少除法运算
}

template <typename T>
inline void Series<T>::for_each( typename Series<T>::visitor_t& v_ ) {
    for ( int loop = _begin; loop != _end; loop = ( loop + 1 ) % _total ) {
        if ( !v_( _values[ loop ] ) ) break;
    }
}

template <typename T>
inline void Series<T>::update( int index_, const element_t& t_ ) {
    _values[ map( index_ ) ] = t_;
}

template <typename T>
inline bool Series<T>::is_valid_slot( int slot_ ) {
    if ( 0 == size() ) return false;

    auto end = _end;
    if ( _begin > _end ) {
        end += _total;
        slot_ += _total;
    }

    return slot_ >= _begin && slot_ < end;
}

template <typename T>
inline typename Series<T>::element_t& Series<T>::operator[]( int index_ ) {
    return get( index_ );
}

template <typename T>
inline typename Series<T>::element_t& Series<T>::get( int index_ ) {
    return _values[ map( index_ ) ];
}

template <typename T>
inline typename Series<T>::element_t& Series<T>::at( int index_ ) {
    return get( index_ );
}

template <typename T>
inline int Series<T>::map( int index_ ) {
    if ( index_ >= size() ) {
        throw std::range_error( "series index overflow" );
    }

    int index = _end - index_ - 1;
    return index < 0 ? index + _total : index;
}

NVX_NS_END

#endif /* A37995CF_4ECF_464C_B471_04E26CC7055C */
