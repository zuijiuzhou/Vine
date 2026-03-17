#pragma once

#include <vine/vi_global.hpp>

#ifdef V_MATH_LIB
#    define V_MATH_API __API_EXPORT__
#else
#    define V_MATH_API __API_IMPORT__
#endif

#define V_MATH_NS_BEGIN                                                                                                                                        \
    V_ROOT_NS_BEGIN                                                                                                                                            \
    namespace math                                                                                                                                             \
    {

#define V_MATH_NS_END                                                                                                                                          \
    V_ROOT_NS_END                                                                                                                                              \
    }
