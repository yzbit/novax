#include "data.h"

CUB_NS_BEGIN

Data& Data::instance() {
    static Data data;

    return data;
}

Data::Data() {
}

Data::~Data() {
}

int Data::subscribe( const code_t& code_ ) {
    return 0;
}

int Data::unsubscribe( const code_t& code_ ) {
    return 0;
}

CUB_NS_END
