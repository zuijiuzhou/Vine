#pragma once

#include <vine/vi_global.hpp>

#ifdef V_VSG_LIB
#    define V_VSG_API V_EXPORT
#else
#    define V_VSG_API V_IMPORT
#endif

#define V_VSG_NS_BEGIN                                                                                                                                    \
    namespace V_ROOT_NS                                                                                                                                   \
    {                                                                                                                                                     \
    namespace vsg                                                                                                                                         \
    {

#define V_VSG_NS_END                                                                                                                                      \
    }                                                                                                                                                     \
    }
