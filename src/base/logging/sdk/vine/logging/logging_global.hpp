#pragma once

#include <vine/vi_global.hpp>

#ifdef V_LOGGING_LIB
#    define V_LOGGING_API V_EXPORT
#else
#    define V_LOGGING_API V_IMPORT
#endif

#define V_LOGGING_NS_BEGIN \
    V_ROOT_NS_BEGIN        \
    namespace logging      \
    {

#define V_LOGGING_NS_END \
    }                    \
    V_ROOT_NS_END
