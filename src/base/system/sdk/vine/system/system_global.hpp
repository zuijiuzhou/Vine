#pragma once

#include <vine/vi_global.hpp>

#ifdef V_SYSTEM_LIB
#    define V_SYSTEM_API V_EXPORT
#else
#    define V_SYSTEM_API V_IMPORT
#endif

#define V_SYSTEM_NS_BEGIN                                                                                                                                      \
    namespace V_ROOT_NS                                                                                                                                        \
    {                                                                                                                                                          \
    namespace system                                                                                                                                           \
    {

#define V_SYSTEM_NS_END                                                                                                                                        \
    }                                                                                                                                                          \
    }
