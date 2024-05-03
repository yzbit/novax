#include <novax.h>

struct TestStrategy : NVX_NS::IStrategy {
    void init() override {
        // QUANT.data()->market()->subscribe( "rb2410" );
    }

    void invoke( NVX_NS::Context* c_ ) {
    }

    void prefight( NVX_NS::Context* c_ ) {
    }
};