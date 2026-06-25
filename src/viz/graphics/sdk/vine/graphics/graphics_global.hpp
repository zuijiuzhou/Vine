#pragma once

#include <vine/vi_global.hpp>

#ifdef V_GRAPHICS_LIB
#    define V_GRAPHICS_API V_EXPORT
#else
#    define V_GRAPHICS_API V_IMPORT
#endif

#define V_GRAPHICS_NS_BEGIN                                                                                                                                    \
    namespace V_ROOT_NS                                                                                                                                        \
    {                                                                                                                                                          \
    namespace graphics                                                                                                                                         \
    {

#define V_GRAPHICS_NS_END                                                                                                                                      \
    }                                                                                                                                                          \
    }
