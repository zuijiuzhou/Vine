#pragma once

#include <vine/vi_global.hpp>

#ifdef V_GRAPHICS_LIB
#    define V_GRAPHICS_API __API_EXPORT__
#else
#    define V_GRAPHICS_API __API_IMPORT__
#endif

#define V_GRAPHICS_NS_BEGIN                                                                                                                                    \
    namespace V_ROOT_NS                                                                                                                                        \
    {                                                                                                                                                          \
    namespace graphics                                                                                                                                         \
    {

#define V_GRAPHICS_NS_END                                                                                                                                      \
    }                                                                                                                                                          \
    }
