#ifndef B51B8BF1_EFFE_4FD1_94C3_9C7FFB93D609
#define B51B8BF1_EFFE_4FD1_94C3_9C7FFB93D609
#include <list>

#include "models.h"
#include "ns.h"

CUB_NS_BEGIN

struct Indicator;
struct Aspect {
    void update( const quotation_t& q_ );

    Indicator* attach( const string_t& name_, const arg_pack_t& args_ );
    int        attach( Indicator* i_ );

private:
    // 按照优先级把指标进行排序，a如果创建了b，那么b显然应该在前面，也就是先计算，但是，如果出现了循环依赖那就没办法了
    std::list<Indicator*> _algos;
};

CUB_NS_END

#endif /* B51B8BF1_EFFE_4FD1_94C3_9C7FFB93D609 */
