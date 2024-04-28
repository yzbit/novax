// broker agent
#include "../core/proxy.h"

// 发送订单信息是req,但是返回的消息还是要pub/sub
// 开启两个模式 ?或者是否有第三种模式
// 这里只负责下单,无法对订单状态进行追踪,追踪需要在各个quant实例的ordermgmt中进行
NVX_NS_BEGIN

struct BaProxy : IBroker {
    static int run() {
        return 0;
    }

    int process() {
        return 0;
    }
};

NVX_NS_END

int main() {
    return NVX_NS::BaProxy::run();
}