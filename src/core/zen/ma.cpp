#include <stdio.h>

#include "ma.h"

CUB_NS_BEGIN

Ma* Ma::create( const ArgPack& p_ ) {
    return new Ma( p_[ 0 ], ( int )p_[ 1 ] );
}

Ma::Ma( const std::string& code_, int period_ ) {
    fprintf( stderr, "%s %d\n", code_.c_str(), period_ );
}

CUB_NS_END