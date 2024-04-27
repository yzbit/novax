#ifndef C4704959_F4BE_4EAA_9C19_9CD09E83DA7D
#define C4704959_F4BE_4EAA_9C19_9CD09E83DA7D
#include "ns.h"

NVX_NS_BEGIN

struct Strategy;
struct Quant {
    static Quant* create();

    virtual ~Quant() {}
    virtual int execute( Strategy* s_ ) = 0;

protected:
    Quant() {}
};

NVX_NS_END

#endif /* C4704959_F4BE_4EAA_9C19_9CD09E83DA7D */
