#pragma once

#include <vine/vi_global.hpp>

#ifdef V_RUNTIME_LIB
#    define V_RUNTIME_API V_EXPORT
#else
#    define V_RUNTIME_API V_IMPORT
#endif

#define V_RUNTIME_NS_BEGIN                                                                                                                                     \
    namespace V_ROOT_NS                                                                                                                                        \
    {                                                                                                                                                          \
    namespace runtime                                                                                                                                          \
    {

#define V_RUNTIME_NS_END                                                                                                                                       \
    }                                                                                                                                                          \
    }
