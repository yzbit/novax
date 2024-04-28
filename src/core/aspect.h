#ifndef B51B8BF1_EFFE_4FD1_94C3_9C7FFB93D609
#define B51B8BF1_EFFE_4FD1_94C3_9C7FFB93D609
#include <vector>

#include "definitions.h"
#include "models.h"
#include "ns.h"

NVX_NS_BEGIN

struct Kline;
struct Aspect final {
    Aspect() = default;
    ~Aspect();

    void update( const quotation_t& q_ );
    int  load( const code_t& code_, const period_t& p_, int count_ );

    // Indicator*    addi( const string_t& name_, const arg_pack_t& args_ );
    int           addi( Indicator* i_ );
    const code_t& code() const;
    Kline&        kline( kidx_t index_ = 0 );
    bool          loaded() const;

    //--
    void debug();

private:
    struct prii_t {
        int        p;
        Indicator* i;
    };

    std::vector<prii_t> _algos;

    int    _ref_prio = 1;
    code_t _symbol   = "";
    Kline* _k        = nullptr;
};

struct AspRepo {
    static AspRepo& instance();
    Aspect*         add( const code_t& code_, const period_t& p_, int count_ );

private:
    using repo_t = std::vector<Aspect>;
    repo_t _repo;
};

NVX_NS_END

#define ASP AspRepo::instance()

#endif /* B51B8BF1_EFFE_4FD1_94C3_9C7FFB93D609 */
