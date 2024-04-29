// data center
#include <map>

#include "../core/proxy.h"

//pub-sub模式
NVX_NS_BEGIN

struct Subscriber {
    int    token_id;
    code_t symbol;
};

struct DcProxy : IData {
    static int run() {
        return 0;
    }

    void update( const quotation_t& tick_ ) override {
        // 根据订阅信息把数据发给订阅者
    }

    void process() {
        switch ( cmd ) {
        case start:
            _m = new ctp::CtpExMd( d_ );
            break;
        case subscribe:
            break;
        case unsubscribe:
            break;
        };
    }

private:
    IMarket* _m;
};

NVX_NS_END

int main() {
    return NVX_NS::DcProxy::run();
}