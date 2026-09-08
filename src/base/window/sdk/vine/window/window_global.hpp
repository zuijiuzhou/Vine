#pragma once

#include <vine/vi_global.hpp>

#ifdef V_WINDOW_LIB
#    define V_WINDOW_API V_EXPORT
#else
#    define V_WINDOW_API V_IMPORT
#endif

#define V_WINDOW_NS_BEGIN                                                                                                                       \
    namespace V_ROOT_NS                                                                                                                         \
    {                                                                                                                                           \
    namespace window                                                                                                                            \
    {

#define V_WINDOW_NS_END                                                                                                                         \
    }                                                                                                                                           \
    }
