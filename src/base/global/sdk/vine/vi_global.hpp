#pragma once

#ifdef __GNUC__
#    define __GCC__
#elif defined(__clang__)
#    define __CLANG__
#elif defined(_MSC_VER)
#    define __MSVC__
#elif defined(__INTEL_COMPILER)
#    define __INTELC__
#else
#    error Unknown compiler.
#endif

#if defined(__MSVC__) || defined(_WIN32) || defined(_WINDOWS)
#    define __API_EXPORT__ __declspec(dllexport)
#    define __API_IMPORT__ __declspec(dllimport)
#else
#    define __API_EXPORT__
#    define __API_IMPORT__
#endif

#if defined(__LP64__) || defined(_LP64) || defined(_WIN64) || defined(__x86_64__)
#    define __VI64__
#endif

#ifndef VI_ROOT_NS
#    define VI_ROOT_NS vine
#endif

#define VI_ROOT_NS_BEGIN                                                                                               \
    namespace VI_ROOT_NS                                                                                               \
    {

#define VI_ROOT_NS_END }

#define VI_DISABLE_COPY(ClassName)                                                                                     \
  private:                                                                                                             \
    ClassName(const ClassName&)            = delete;                                                                   \
    ClassName& operator=(const ClassName&) = delete;

#define VI_DISABLE_MOVE(ClassName)                                                                                     \
  private:                                                                                                             \
    ClassName(ClassName&&)            = delete;                                                                        \
    ClassName& operator=(ClassName&&) = delete;

#define VI_DISABLE_COPY_MOVE(ClassName)                                                                                \
    VI_DISABLE_COPY(ClassName)                                                                                         \
    VI_DISABLE_MOVE(ClassName)
