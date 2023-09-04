#pragma once

#include "Std.h"
#include "Platform.h"

#define VI_DISABLE_COPY(Cls) \
    Cls(const Cls &) = delete;\
    Cls &operator=(const Cls &) = delete;

#define VI_DISABLE_MOVE(Cls) \
    Cls(Class &&) = delete; \
    Cls &operator=(Cls &&) = delete;

#define VI_DISABLE_COPY_MOVE(Cls) \
    VI_DISABLE_COPY(Cls) \
    VI_DISABLE_MOVE(Cls)

#pragma once

#define VI_ENUM_CLASS_FLAGS(Enum) \
inline constexpr Enum& operator|=(Enum& x, Enum y) { \
    return x = static_cast<Enum>( \
        static_cast<std::underlying_type_t<Enum>>(x) | static_cast<std::underlying_type_t<Enum>>(y) \
); } \
inline constexpr Enum& operator&=(Enum& x, Enum y) { \
    return x = static_cast<Enum>( \
        static_cast<std::underlying_type_t<Enum>>(x) & static_cast<std::underlying_type_t<Enum>>(y) \
); } \
inline constexpr Enum& operator^=(Enum& x, Enum y) { \
    return x = static_cast<Enum>( \
        static_cast<std::underlying_type_t<Enum>>(x) ^ static_cast<std::underlying_type_t<Enum>>(y) \
); } \
inline constexpr Enum operator| (Enum x, Enum y) { \
    return static_cast<Enum>( \
        static_cast<std::underlying_type_t<Enum>>(x) | static_cast<std::underlying_type_t<Enum>>(y) \
); } \
inline constexpr Enum operator& (Enum x, Enum y) { \
    return static_cast<Enum>( \
        static_cast<std::underlying_type_t<Enum>>(x) & static_cast<std::underlying_type_t<Enum>>(y) \
); } \
inline constexpr Enum operator^ (Enum x, Enum y) { \
    return static_cast<Enum>( \
        static_cast<std::underlying_type_t<Enum>>(x) ^ static_cast<std::underlying_type_t<Enum>>(y) \
); } \
inline constexpr bool operator! (Enum x) { \
    return !static_cast<std::underlying_type_t<Enum>>(x); \
} \
inline constexpr Enum operator~ (Enum x) { \
    return static_cast<Enum>(~static_cast<std::underlying_type_t<Enum>>(x)); \
}

template<typename Enum>
inline constexpr bool EnumHasFlag(Enum a, Enum b)
{
    return static_cast<bool>( static_cast<std::underlying_type_t<Enum>>(a & b) );
}