#include "data.h"

#include "ctp_proxy.h"
#include "market.h"

CUB_NS_BEGIN

Data& Data::instance() {
    static Data data;

    return data;
}

Data::Data() {
    _market = new CtpExMd();
}

Data::~Data() {
}

int Data::subscribe( const code_t& code_ ) {
    return _market->subscribue( code_ );
}

int Data::unsubscribe( const code_t& code_ ) {
    return _market->unsubscribue( code_ );
}

CUB_NS_END
