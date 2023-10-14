#ifndef B51B8BF1_EFFE_4FD1_94C3_9C7FFB93D609
#define B51B8BF1_EFFE_4FD1_94C3_9C7FFB93D609
#include <list>

#include "definitions.h"
#include "models.h"
#include "ns.h"

CUB_NS_BEGIN

struct Kline;
struct Indicator;
struct Aspect {
    Aspect() = default;
    ~Aspect();

    void update( const quotation_t& q_ );
    int  load( const code_t& code_, const period_t& p_, int count_ );

    Indicator*    addi( const string_t& name_, const arg_pack_t& args_ );
    int           addi( Indicator* i_ );
    const code_t& code() const;
    Kline&        kline();
    bool          loaded() const;

    //--
    void debug();

private:
    struct prii_t {
        int        p;
        Indicator* i;
    };

    std::list<prii_t> _algos;

    int    _ref_prio = 1;
    code_t _symbol   = "";
    Kline* _k        = nullptr;
};

inline Kline& Aspect::kline() {
    return *_k;
}

inline const code_t& Aspect::code() const {
    return _symbol;
}

inline bool Aspect::loaded() const {
    return !_symbol.empty();
}

CUB_NS_END

#endif /* B51B8BF1_EFFE_4FD1_94C3_9C7FFB93D609 */
