#include <iostream>
#include <stdio.h>

#include "../core/data.h"
#include "../core/dci_role.h"
#include "../core/log.hpp"
#include "../core/models.h"

struct XContext : cub::DataContext {
    void update( const cub::quotation_t& q_ ) {
        std::cout << "update: q.ask=" << q_.ask << std::endl;
    }

    void invoke() {
        std::cout << "invoke" << std::endl;
    }
};

int main() {
    LOG_INIT( "./log/testasp", -1 );
    LOG_ENABLE_STDOUT();

    cub::quotation_t q;
    q.ask = 99.99;

    auto d = new cub::Data( new XContext() );
    d->start();
    d->stop();

    d->subscribe( "rb2401" );
    //d->unsubscribe( "rb2410" );

    d->update( q );

    fprintf( stderr, "case finish\n" );

    for ( ;; ) {
        fprintf( stderr, "-\n" );
        ::sleep( 1 );
    }
    return 0;
}