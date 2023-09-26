#ifndef F4D9DAEC_B9A1_48EF_8FEB_A99D60585AD3
#define F4D9DAEC_B9A1_48EF_8FEB_A99D60585AD3
#include <comm/definitions.h>
#include <memory>
#include <comm/ns.h>

#include "indicator.h"
#include "market.h"

CUB_NS_BEGIN

struct Data {
    Data& instance();

    Data();
    virtual ~Data();

    int subscribe( const code_t& code_ );
    int unsubscribe( const code_t& code_ );

private:
    std::unique_ptr<Market>    _market;
    std::unique_ptr<Indicator> _indicator;
};

CUB_NS_END
#define DATA cub::data::instance()

#endif /* F4D9DAEC_B9A1_48EF_8FEB_A99D60585AD3 */
