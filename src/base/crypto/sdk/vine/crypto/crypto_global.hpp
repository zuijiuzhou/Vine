#pragma once

#include <vine/vi_global.hpp>

#ifdef V_CRYPTO_LIB
#    define V_CRYPTO_API V_EXPORT
#else
#    define V_CRYPTO_API V_IMPORT
#endif

/**
 * @brief vine::crypto provides cryptographic algorithms only.
 *
 * Scope: hashes (MD5/SHA-1/SHA-256/CRC32), ciphers (AES/RSA/ChaCha20) and
 * HMAC. Key management and certificates belong to higher layers (appfw).
 */
#define V_CRYPTO_NS_BEGIN                                                                                                                               \
    namespace V_ROOT_NS                                                                                                                                 \
    {                                                                                                                                                   \
    namespace crypto                                                                                                                                    \
    {

#define V_CRYPTO_NS_END                                                                                                                                 \
    }                                                                                                                                                   \
    }
