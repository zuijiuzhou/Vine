#pragma once

#include <vine/vi_global.hpp>

#ifdef V_GEOMETRY_LIB
#    define V_GEOMETRY_API __API_EXPORT__
#else
#    define V_GEOMETRY_API __API_IMPORT__
#endif

#define V_GEOMETRY_NS_BEGIN                                                                                                                                    \
    V_ROOT_NS_BEGIN                                                                                                                                            \
    namespace geometry                                                                                                                                         \
    {

#define V_GEOMETRY_NS_END                                                                                                                                      \
    V_ROOT_NS_END                                                                                                                                              \
    }
