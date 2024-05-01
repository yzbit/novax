#include <stdio.h>
#include <string.h>

#include "../core/series.h"

int main() {
    auto s = new cub::Series( 40 );
    s->append( 3 );
    s->append( 5.0 );
    s->append( "aaaa" );

    s->append( { 6 } );

    s->for_each( []( auto& e_ ) {
        printf( "%d\n", e_.i );

        return true;
    } );
    printf( "series size=%d,right=%d\n", s->size(), s->at( 0 ).i );

    for ( int i = 0; i < 50; ++i ) {
        s->append( i );
    }

    printf( "series size=%d,right=%d\n", s->size(), s->at( 0 ).i );
    return 0;
}