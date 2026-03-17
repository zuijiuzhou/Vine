#pragma once

#include <vine/vi_global.hpp>

#ifdef V_DI_LIB
#    define V_DI_API __API_EXPORT__
#else
#    define V_DI_API __API_IMPORT__
#endif

#define V_DI_NS_BEGIN                                                                                                                                          \
    namespace V_ROOT_NS                                                                                                                                        \
    {                                                                                                                                                          \
    namespace di                                                                                                                                               \
    {

#define V_DI_NS_END                                                                                                                                            \
    }                                                                                                                                                          \
    }
