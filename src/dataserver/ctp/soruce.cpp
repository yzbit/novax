#include "provider.h"
namespace ctp {
nvx_st start() {
    _td = new trader();
    _md = new mdex();

    return NVX_OK;
}
nvx_st stop() {

    return NVX_OK;
}
}  // namespace ctp