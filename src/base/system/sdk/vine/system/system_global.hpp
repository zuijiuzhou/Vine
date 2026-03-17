#pragma once

#include <vine/vi_global.hpp>

#ifdef V_SYSTEM_LIB
#    define V_SYSTEM_API __API_EXPORT__
#else
#    define V_SYSTEM_API __API_IMPORT__
#endif

#define V_SYSTEM_NS_BEGIN                                                                                                                                      \
    namespace V_ROOT_NS                                                                                                                                        \
    {                                                                                                                                                          \
    namespace system                                                                                                                                           \
    {

#define V_SYSTEM_NS_END                                                                                                                                        \
    }                                                                                                                                                          \
    }
