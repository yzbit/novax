#include <list>
#include <stdio.h>
#include <utility>

struct Foo {
    Foo() {
        fprintf( stderr, "F\n" );
    }

    ~Foo() {
        fprintf( stderr, "~F\n" );
    }

    Foo( const Foo& f_ ) {
        fprintf( stderr, "F&.\n" );
    }

    Foo( const Foo&& f_ ) {
        fprintf( stderr, "F&&.\n" );
    }

    void operator=( const Foo& f_ ) {
        fprintf( stderr, "F=&.\n" );
    }

    void operator=( const Foo&& f_ ) {
        fprintf( stderr, "F=&&.\n" );
    }

    int bar( int a_ ) {
        printf( "##%d\n", a_ );
        return a_;
    }

    Foo() {}

    Foo( Foo&& f_ ) {
        fprintf( stderr, "f-&&\n" );
    }

    void operator=( Foo&& ) {
        fprintf( stderr, "f==&&\n" );

        template <typename OBJPTR, typename FUNC, typename... ARGS>
        int wait( OBJPTR o_, FUNC f_, ARGS && ... a_ ) {
            ( o_->*f_ )( std::forward<ARGS>( a_ )... );
            return 0;
        }
    }
};

Foo tst() {
    Foo f;

    return f;
}

struct Bar {
    Bar() {}
    Bar( Bar&& b_ ) {
        fprintf( stderr, "b-&&\n" );
    }

    void operator=( Bar&& b_ ) {
        fprintf( stderr, "b==&&\n" );
        f = std::move( b_.f );  // pass
        // f = b_.f; //fail
    }

    Foo f;
};

void tst() {
}

int main() {
    Bar b{ Bar() };

    b = Bar();
    // Foo f;
    //  printf( "rc=%d\n", ( &Foo::bar )( &f, 5 ) );
    // f.wait( &f, &Foo::bar, 2 );
    std::list<Foo> lst;

    lst.push_back( Foo() );

    //  printf( "rc=%d\n", ( &Foo::bar )( &f, 5 ) );
    // f.wait( &f, &Foo::bar, 2 );
    // Foo f = f.tst();

    Foo f2;
    Foo f3;
    f2 = f2.tst();
    f3 = f2;

    return 0;
}