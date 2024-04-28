#ifndef B4872862_3AFF_45FF_86DA_A0808D740978
#define B4872862_3AFF_45FF_86DA_A0808D740978
#include "definitions.h"
#include "models.h"

NVX_NS_BEGIN

struct IIndicator {
    enum class IndiType {
        main,
        addi
    };

    virtual size_t   track_count() { return 1; }
    virtual IndiType type() { return IndiType::main; }
    virtual double   at( int track_, int index_ ) { return .0; }

    virtual ~IIndicator() {}
};

struct IAlgo {
    virtual void calc( const quotation_t& qut_, int total_bars_ ) = 0;

    virtual ~IAlgo() {}
};

// struct Ma:IAlogo, IIndicator
NVX_NS_END

#endif /* B4872862_3AFF_45FF_86DA_A0808D740978 */
