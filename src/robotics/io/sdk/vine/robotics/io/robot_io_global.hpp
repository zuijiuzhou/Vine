#pragma once

#include <vine/robotics/robot_core_global.hpp>

#ifdef V_ROBOTICSIO_LIB
#    define V_ROBOTICS_IO_API V_EXPORT
#else
#    define V_ROBOTICS_IO_API V_IMPORT
#endif

/** @brief Current XML format version (major). */
#define V_ROBOTICS_IO_VERSION_MAJOR 1
/** @brief Current XML format version (minor). */
#define V_ROBOTICS_IO_VERSION_MINOR 0

#define V_ROBOTICS_IO_NS_BEGIN                                                                                                                          \
    V_ROBOTICS_NS_BEGIN                                                                                                                                 \
    namespace io                                                                                                                                        \
    {

#define V_ROBOTICS_IO_NS_END                                                                                                                            \
    }                                                                                                                                                   \
    V_ROBOTICS_NS_END
