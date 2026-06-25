#pragma once

#include <vine/vi_global.hpp>

#ifdef V_GEOMETRY_LIB
#    define V_GEOMETRY_API V_EXPORT
#else
#    define V_GEOMETRY_API V_IMPORT
#endif

#define V_GEOMETRY_NS_BEGIN                                                                                                                                    \
    V_ROOT_NS_BEGIN                                                                                                                                            \
    namespace geometry                                                                                                                                         \
    {

#define V_GEOMETRY_NS_END                                                                                                                                      \
    V_ROOT_NS_END                                                                                                                                              \
    }
