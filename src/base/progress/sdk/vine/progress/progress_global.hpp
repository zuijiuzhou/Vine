#pragma once

#include <vine/vi_global.hpp>

#ifdef V_PROGRESS_LIB
#    define V_PROGRESS_API V_EXPORT
#else
#    define V_PROGRESS_API V_IMPORT
#endif

#define V_PROGRESS_NS_BEGIN \
    V_ROOT_NS_BEGIN         \
    namespace progress      \
    {

#define V_PROGRESS_NS_END \
    }                     \
    V_ROOT_NS_END
