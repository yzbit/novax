#include <stdio.h>
#include <utility>

struct Foo {
    int bar( int a_ ) {
        printf( "##%d\n", a_ );
        return a_;
    }

    template <typename OBJPTR, typename FUNC, typename... ARGS>
    int wait( OBJPTR o_, FUNC f_, ARGS&&... a_ ) {
        ( o_->*f_ )( std::forward<ARGS>( a_ )... );
        return 0;
    }
};

int main() {
    Foo f;
    // printf( "rc=%d\n", ( &Foo::bar )( &f, 5 ) );
    f.wait( &f, &Foo::bar, 2 );

    return 0;
}