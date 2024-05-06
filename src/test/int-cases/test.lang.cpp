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