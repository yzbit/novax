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

int main() {
    printf( "size=%d\n", MaxTypeSize<int, short, long, char, a>::value );

    return 0;
}
