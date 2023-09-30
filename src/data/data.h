#ifndef F4D9DAEC_B9A1_48EF_8FEB_A99D60585AD3
#define F4D9DAEC_B9A1_48EF_8FEB_A99D60585AD3

#include <comm/candle.h>
#include <comm/definitions.h>
#include <cub_ns.h>
#include <memory>
#include <reactor/msg.h>

CUB_NS_BEGIN
class Indicator;
class Market;

struct Data {
    static Data& instance();

    Data();
    virtual ~Data();

    int subscribe( const code_t& code_ );
    int unsubscribe( const code_t& code_ );

private:
    void on_data( const quotation_t& tick_ );
    void on_msg( const msg::header_t& h );

private:
    Market*    _market    = nullptr;
    Indicator* _indicator = nullptr;
};

CUB_NS_END
#define DATA cub::Data::instance()

#endif /* F4D9DAEC_B9A1_48EF_8FEB_A99D60585AD3 */
