#pragma once

#include <vine/vi_global.hpp>

#ifdef V_MODELIO_LIB
#    define V_MODELIO_API V_EXPORT
#else
#    define V_MODELIO_API V_IMPORT
#endif

#define V_MODELIO_NS_BEGIN    \
    V_ROOT_NS_BEGIN           \
    namespace modelio         \
    {

#define V_MODELIO_NS_END \
    V_ROOT_NS_END        \
    }
