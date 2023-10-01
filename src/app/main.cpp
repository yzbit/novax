
#include <definitions.h>
#include <stdio.h>

#include "application.h"

struct BreakBuy : Algo {
    void on_refresh() {
        if ( aaa ) {
            c->sell();
            c.get_money();
            c->cover();
        }
    }
};

struct Quant : cub::QuantApp {
    int init() override {
        fprintf( stderr, "init quant\n" );
        return cub::QuantApp::init();
    }

    int exec() override {
        fprintf( stderr, "exe quant\n" );
        return cub::QuantApp::exec();
    }
};

int main() {
    Context& c = Context::initialize();

    auto asp = c.createt( "rb2410", cub::period_t{ cub::period_t::type_t::hour, 1 }, 40, "1hour" );

    id_t wm   = asp->attach( wmi );
    id_t ma1  = asp->attach( "ma", { 1, 2, 3, 4 } );
    id_t psi1 = asp->attach( "psi", { 27, 12, 3 } );

    c.load( "break", { 1, 2, 3, 4 } );
    // c.load( algo );

    return c.exec();

#if 0
    Quant* q = new Quant();

    //! 交易系统初始化要做哪些事情：
    /*!
    1）初始化reactor进行进行初始--其实没必要
    2）设置日志
    3）设置数据网关和交易网关--可能通过插件提供
    4）加载和初始化策略
    5）订阅数据----感觉订阅数据和加载失败都应该是在加载策略的时候做的

    //本质上来说策略和指标没太大区别，他应该也在指标模块计算，##只是计算层次是最后一层罢了##, 只是指标不允许下单，策略可以下单。既然如此，那么trade模块就不应该暴露出来，否则任何人都可以调用
    //应该把计算和数据分开,algo是计算，facade是数据
    //计算包含上下文
    //数据包含series，或者
    struct Indicator:Algo{
    };


    struct Ma: Indicator{

    };

    //--这实际是一个facade类-提供了对trader，data，account等业务对象的封装
    struct Strategy:Algo{
        virtual void on_calc(){
            on_refresh();
        }
    };

    struct BreakThrough: Strategy{
        void on_refresh(ctx){
            if(ssss){
                sell_short("rb2410",10,"");
            }

            if(){
                close_all();
            }

            revert()

            money = total_balance();
            if(money > 124){

            }

            aspect*a =ctx->aspect("k1");

            a->get_names();//psi1,ma123
            int bar_index = a->curr_bar();
            int bars = a->total_bars();

            if(a->get("psi",1,0) > a->get("ma","1111")){
                buy();
            }

            a->k()->llv(4);
            a->llv();
        }
    };

    //#每次只能加载一个策略#
    //作为一个交易系统而言，交易策略应该运行的时候加载，而不是通过修改源代码来加载
    //

    q->attach(s1);
    */

    q->init();

    return q->exec();
#endif
}