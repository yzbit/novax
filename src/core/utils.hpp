#ifndef A0091236_F594_4A70_BDCB_927CD411D38C
#define A0091236_F594_4A70_BDCB_927CD411D38C
#include <functional>
#include <stdint.h>
#include <stdio.h>

#include "ns.h"

CUB_NS_BEGIN

inline void dumpHex(
    const uint8_t*                          data_,
    size_t                                  length_,
    int                                     size_per_row_,
    std::function<void( const char* hex_ )> print_ = []( const char* hex_ ) { printf( "%s", hex_ ); } ) {

    char hex[ 8 ];
    for ( int i = 0; i < ( int )length_; ++i ) {
        if ( i != 0 && i % size_per_row_ == 0 ) {
            print_( "\n" );
        }
        sprintf( hex, "%02X ", data_[ i ] );
        print_( hex );
    }

    print_( "\n" );
}

CUB_NS_END

#endif /* A0091236_F594_4A70_BDCB_927CD411D38C */
