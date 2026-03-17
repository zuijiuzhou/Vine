#pragma once

#include <vine/vi_global.hpp>

#ifdef V_RUNTIME_LIB
#    define V_RUNTIME_API __API_EXPORT__
#else
#    define V_RUNTIME_API __API_IMPORT__
#endif

#define V_RUNTIME_NS_BEGIN                                                                                                                                     \
    namespace V_ROOT_NS                                                                                                                                        \
    {                                                                                                                                                          \
    namespace runtime                                                                                                                                          \
    {

#define V_RUNTIME_NS_END                                                                                                                                       \
    }                                                                                                                                                          \
    }
