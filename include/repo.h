#ifndef BCCAE25F_6EDE_48A1_9378_1C7A196F0A43
#define BCCAE25F_6EDE_48A1_9378_1C7A196F0A43

#include <vector>

#include "ns.h"

NVX_NS_BEGIN
template <typename T, typename class container = std::vector>
struct repo {
    void add();
    void del();
    void clear();
    
};

NVX_NS_END

#endif /* BCCAE25F_6EDE_48A1_9378_1C7A196F0A43 */
