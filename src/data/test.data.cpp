#include <cub.h>
#include <stdio.h>

#include "algo/ma.h"

int main() {
    DATA.subscribe( "rb2410" );
    cub::Ma::create( { _( "rb1410" ), 2, 3, 4 } );

    return 0;
}