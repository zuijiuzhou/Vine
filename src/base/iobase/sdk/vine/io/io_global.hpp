#pragma once

#include <vine/vi_global.hpp>

#ifdef V_IOBASE_LIB
#    define V_IOBASE_API V_EXPORT
#else
#    define V_IOBASE_API V_IMPORT
#endif

/**
 * @brief vine::io provides byte/stream/encoding primitives.
 *
 * Scope: byte buffers, input/output streams, gzip/zlib (de)compression and
 * encodings (base64/hex). File-system path operations are out of scope; they
 * belong to the system module.
 */
#define V_IO_NS_BEGIN                                                                                                                                   \
    namespace V_ROOT_NS                                                                                                                                 \
    {                                                                                                                                                   \
    namespace io                                                                                                                                        \
    {

#define V_IO_NS_END                                                                                                                                     \
    }                                                                                                                                                   \
    }
