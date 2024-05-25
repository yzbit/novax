#include <cstring>
#include <stdio.h>
#include <type_traits>

template <typename T, typename = std::enable_if_t<std::is_array_v<T>>>
struct Encaps {
    enum { LENGTH = sizeof( T ) };

    Encaps() = default;
    Encaps( const T& ex_ ) {
        memcpy( _data, &ex_, LENGTH );
        _data[ LENGTH ] = 0;
    }

    bool operator==( const Encaps& r_ ) {
        return ( this == &r_ ) || ( 0 == memcmp( _data, r_._data, LENGTH ) );
    }

    bool operator!=( const Encaps& r_ ) {
        return ( this != &r_ ) && ( 0 != memcmp( _data, r_._data, LENGTH ) );
    }

    bool operator==( const T& r_ ) {
        return 0 == memcmp( _data, r_, LENGTH );
    }

    bool operator!=( const T& r_ ) {
        return 0 != memcmp( _data, r_, LENGTH );
    }

    Encaps& operator=( const Encaps& r_ ) {
        if ( this == &r_ ) return *this;

        memcpy( _data, r_._data, sizeof( _data ) );
        return *this;
    }

    Encaps& operator=( const T& r_ ) {
        memcpy( _data, r_, Encaps<T>::LENGTH );
        _data[ Encaps<T>::LENGTH ] = 0;

        return *this;
    }

    bool is_valid() {
        return !!_data[ 0 ];
    }

protected:
    char _data[ LENGTH + 1 ];
};
using atype = char[ 10 ];

struct ref_t : Encaps<atype> {
    ref_t( const atype& t_ )
        : Encaps( t_ ) {}
    ref_t() = default;
};

int main() {
    atype a, b;
    strcpy( a, "aaa" );
    strcpy( b, "bbb" );

    ref_t r( a );
    ref_t r0( b );
    ref_t r1;

    r = r0;

    printf( "r0 equals r: %d\n", r == r0 );

    return 0;
};