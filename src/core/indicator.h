#ifndef B4872862_3AFF_45FF_86DA_A0808D740978
#define B4872862_3AFF_45FF_86DA_A0808D740978
/*
可能有多组只,每组值都是一个series但是类型可能不一样
要不要一个基准指标,比如5分钟,10分钟, 像ui一样, 没有基准的话,如何知道当前是第几根呢

考虑ui特殊性，只能有一个base，我们没有ui的限制的情况下，每个base都是平等的，但是即便如此，每个Algo必然要基于某个Kline吗
没有基准虽然可以做的很灵活，但是使用起来一样很麻烦--为了理解这其中的逻辑

我们可以认为所有的指标必然都是来自于市场价格，也就是来自于K线，当然也可能来自多组K线，比如我有一个指标分别来1分钟和5分钟x线

g = f(kline-1,kline-5)

那么g如何和kline-1，kline-5的数组索引对应呢
或者我获取g[n]的值表示什么，有没有几何或者物理意义（没有的就很难理解，也就不会创造和使用）


如果我们认为技术指标和ui一样只能在某一个k线的基础上建立，那就会很简单


每一组(k,i1,i2,i3,i4)组成一个facade或者cluster

订阅某个周期的k线的时候先会自动创建一个facade，然后调用 facade->add_indicator();
当然某个指标可以计算的时候访问多个facade，因为本质上指标和策略没什么太大区别

facade都是可以##并行计算##的

计算完facade就可以再计算交叉的指标g
那么这个指标要想获取某个值，只能是自己实现了，重载
get(int track_,int index_){
}

计算的时候可以得到指标 g=F(facade1-i1,facade2-2)
可以叫 aspect

//这样会自动订阅k线----研究一下c++的自动并行计算
//todo default aspect包含所有不以k线为基准的aspect


Aspect *a = create_aspect(name,symbol,period);
a->add_indicator("ma",{1,2,3,4});
a->add_indicator("custom",{1,2,3,4});

a = context->owner //当前指标所在的aspect-可以作为algo成员
a = context->aspect(name);
default_as = context->aspect();
default_as = context->aspect("");


add_indicator() <=> default_as->add_indicator

algo::aspect() 自己所属的aspect
context::aspect() 系统default aspect
不同的aspect之间自然可以互相引用的，那么就很难并行计算

如果aspect之间可以相互引用，那就有计算先后的问题，a1会在运算的过程种动态的引用a2，那么我怎么知道a1需要在a2后面计算呢

除非限制aspect之间绝对不可以互相引用，（约定），除了default——aspect，他可以最后计算
或者严格遵守先算小k线再算大k的原则,也就是小k线引用大k线的值似乎也没啥大问题，毕竟大k反应更慢一些

aspect 之间可以比较
a1>a2
##或者我们可以在每次一更新之前把所有的aspect都记成dirty，如果计算过则更新成updated##
如果我们依赖的ax 没有计算，可以把自己推迟，而把ax提前，
函数
context->aspect("")中判断是当前是谁在调用(context->caller)，如果此时aspect还未更新，则等在这里等他更新完了再返回，但是这可能引起风暴反应，
calll: a1->a2-->a3-- 然后a3又依赖于caller或者a2，那就麻烦大了，此时我们会生成
call->a1->a2->a3->a2 出现了环，此时我们只能是不管了，计算最大的链条即可
此时get操作需要等a2和a3计算完，这就和并行更难沾上边了--get操作卡在那里也很难受

#我觉得问题不大，因为交叉使用aspect就已经注定了数据无法对齐#




应该可以通过名字直接引用当前aspect内的指标
###指标创建的时候可以通过名字来索引，这可以保证唯一性

algo *a = indicator("ma-1231");
algo *a1 = context->aspect(name)->indicator("rsi-11");

if(a > a1){
    TRADE.sell_short();
}
else{
    TRADE.close_all();
}

amnt_t total_money = ACCOUNT.captical();
auto le = ACCOUNT.ganggan();
*/

#include <unordered_map>

#include "definitions.h"
#include "models.h"
#include "ns.h"

#define MAX_INDICATOR_PRIO -1

CUB_NS_BEGIN
struct Aspect;
struct Series;

struct Indicator {
    static Indicator* create( const string_t& name_, const arg_pack_t& args_, Aspect* asp_ );

    virtual void on_init()                        = 0;
    virtual void on_calc( const quotation_t& q_ ) = 0;
    virtual int  series_cnt() { return 0; };

    int  prio();
    void set_prio( int p_ );

protected:
    Series* add_series( int track_, int size_ );
    Series* track( int index_ = 0 );

private:
    void    set_asp( Aspect* asp_ ) { _asp = asp_; }
    Aspect* asp() { return _asp; }

private:
    using series_repo_t = std::unordered_map<int, Series*>;

    series_repo_t _series;
    Aspect*       _asp;

    // 创建的越晚优先级越高--需要先计算，当我们创建A的时候，A可能创建B,然后B要先计算然后再计算A-这并不绝对，但是也是最简单的处理方式
    int _prio;
};

CUB_NS_END

#endif /* B4872862_3AFF_45FF_86DA_A0808D740978 */
