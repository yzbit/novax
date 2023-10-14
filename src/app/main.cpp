
#include <definitions.h>
#include <quant.h>
#include <stdio.h>
#include <strategy.h>

#include "../gallery/break_through.h"

/*
   1）初始化reactor进行进行初始--其实没必要
   2）设置日志
   3）设置数据网关和交易网关--可能通过插件提供
   4）加载和初始化策略
   5）订阅数据----感觉订阅数据和加载失败都应该是在加载策略的时候做的
*/

int main() {
    cub::Quant* q = cub::Quant::create();

    cub::Strategy* s = new BreakTh( "rb2410" );

    return q->execute( s );
}