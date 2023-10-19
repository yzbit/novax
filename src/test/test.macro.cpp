#include <stdio.h>

#define DECL_SYNC_OBJECT\
private:\

#define SYNC_WAIT()
#define SYNC_UPDATE()

#define wait_call( _func_, _timeout_, _callback_ ) \
    do {                                           \
        _func_;                                    \
        printf( "%u\n", _timeout_ );               \
        _callback_();                              \
    } while ( 0 )

void foo() {
    printf( "foo=\n" );
}

int main() {

    wait_call( foo(), 100, foo );

    return 0;
}