#include <stdio.h>

#include "../core/algo_repo.h"
#include "../core/aspect.h"
#include "../core/definitions.h"
#include "../core/indicator.h"

int main() {
    auto* a = new cub::Aspect();

    a->attach( ALGO_NAME( KLINE ), { "rb2410", cub::period_t{ cub::period_t::type_t::min, 1 }, 40 } );

    delete a;

    return 0;
}