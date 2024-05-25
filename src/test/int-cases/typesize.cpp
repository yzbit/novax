#include <stdio.h>

template <typename... T>
struct MaxTypeSize;

template <>
struct MaxTypeSize<> {
    enum {
        value = 0
    };
};
struct a {
    long x, y, z;
};

template <typename T, typename... R>
struct MaxTypeSize<T, R...> {
    enum {
        value = sizeof( T ) > ( MaxTypeSize<R...>::value ) ? sizeof( T ) : ( MaxTypeSize<R...>::value )
    };
};

struct C {
    C() {
        printf( "a[1]=%d\n", aa[ 1 ] );
    }

    int aa[ 8 ];
};

int main() {
    printf( "size=%d\n", MaxTypeSize<int, short, long, char, a>::value );

    int aa[ 9 ];// = { 0 };

    printf( "a[1]=%d\n", aa[ 1 ] );

    C c;
    return 0;
}
