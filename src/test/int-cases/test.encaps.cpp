#include <cstring>
#include <stdio.h>
#include <type_traits>

template <typename T, typename = std::enable_if_t<std::is_array_v<T>>>
struct encpas {
    enum { LENGTH = sizeof( T ) };

    encpas() = default;
    encpas( const T& ex_ ) {
        memcpy( _data, &ex_, LENGTH );
        _data[ LENGTH ] = 0;
    }

    bool operator==( const encpas& r_ ) {
        return ( this == &r_ ) || ( 0 == memcmp( _data, r_._data, LENGTH ) );
    }

    bool operator!=( const encpas& r_ ) {
        return ( this != &r_ ) && ( 0 != memcmp( _data, r_._data, LENGTH ) );
    }

    bool operator==( const T& r_ ) {
        return 0 == memcmp( _data, r_, LENGTH );
    }

    bool operator!=( const T& r_ ) {
        return 0 != memcmp( _data, r_, LENGTH );
    }

    encpas& operator=( const encpas& r_ ) {
        if ( this == &r_ ) return *this;

        memcpy( _data, r_._data, sizeof( _data ) );
        return *this;
    }

    encpas& operator=( const T& r_ ) {
        memcpy( _data, r_, encpas<T>::LENGTH );
        _data[ encpas<T>::LENGTH ] = 0;

        return *this;
    }

    bool is_valid() {
        return !!_data[ 0 ];
    }

protected:
    char _data[ LENGTH + 1 ];
};
using atype = char[ 10 ];

struct ordref : encpas<atype> {
    ordref( const atype& t_ )
        : encpas( t_ ) {}
    ordref() = default;
};

int main() {
    atype a, b;
    strcpy( a, "aaa" );
    strcpy( b, "bbb" );

    ordref r( a );
    ordref r0( b );
    ordref r1;

    r = r0;

    printf( "r0 equals r: %d\n", r == r0 );

    return 0;
};