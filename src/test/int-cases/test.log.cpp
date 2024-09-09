#include "../../core/log.hpp"

int main() {

    nvx::logz::conf c{ 0 };

    c.dir        = "./";
    c.prefix     = "test";
    c.rot_size   = 1024;
    c.time_fmt   = 0;
    c.use_stdout = 1;
    c.in_place   = 1;

    LOGZ_INIT( c );

    int i = 100;
    while ( i-- ) {
        LOGZ_FMT( "put,>>>>>>> i = %d", i );
        sleep( 1 );
    }

    LOGZ_CLOSE();

    return 0;
}