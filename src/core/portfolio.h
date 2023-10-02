#ifndef BA1B1004_8EFD_40A3_866E_1DE656425F40
#define BA1B1004_8EFD_40A3_866E_1DE656425F40
#include <unordered_map>
#include <vector>

#include "definitions.h"
#include "models.h"
#include "ns.h"

CUB_NS_BEGIN

//每个代码应该有一个总仓位和分仓位,不应该全部合并掉,这样会无法查找
struct Instrument {
    int update( vol_t qty_, price_t price_, bool herge_ );

private:
    odir_t  _dir       = odir_t::none;
    vol_t   _vol       = 0;
    money_t _profit    = 0;
    money_t _commssion = 0;

private:
    code_t                  _code;
    std::vector<position_t> _p;
};

struct Portfolio {
    using repo_t = std::unordered_map<code_t, Instrument, code_hash_t>;

    Portfolio();
    static Portfolio& instance();

    int update( const code_t& c_, vol_t qty_, price_t price_ );

private:
    repo_t _r;
};

CUB_NS_END

#define PORTFOLIO cub::Portfolio::instance()
#endif /* BA1B1004_8EFD_40A3_866E_1DE656425F40 */
