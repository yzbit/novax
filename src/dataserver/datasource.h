#ifndef DC76187A_C745_4EAB_92F6_1B70AE94D57D
#define DC76187A_C745_4EAB_92F6_1B70AE94D57D

NVX_NS_BEGIN

struct provider {
    provider( ipub* pub_ );
    virtual ~provider() {}

    virtual nvx_st start() = 0;
    virtual nvx_st stop()  = 0;
};

NVX_NS_END

#endif /* DC76187A_C745_4EAB_92F6_1B70AE94D57D */
