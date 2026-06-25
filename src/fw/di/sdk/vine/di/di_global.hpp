#pragma once

#include <vine/vi_global.hpp>

#ifdef V_DI_LIB
#    define V_DI_API V_EXPORT
#else
#    define V_DI_API V_IMPORT
#endif

#define V_DI_NS_BEGIN                                                                                                                                          \
    namespace V_ROOT_NS                                                                                                                                        \
    {                                                                                                                                                          \
    namespace di                                                                                                                                               \
    {

#define V_DI_NS_END                                                                                                                                            \
    }                                                                                                                                                          \
    }
