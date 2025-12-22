#pragma once

#include <vine/vi_global.hpp>

#include <type_traits>

#ifdef VI_CORE_LIB
#    define VI_CORE_API __API_EXPORT__
#else
#    define VI_CORE_API __API_IMPORT__
#endif

#define VI_CORE_NS_BEGIN VI_ROOT_NS_BEGIN

#define VI_CORE_NS_END VI_ROOT_NS_END

#define VI_ENUM_CLASS_FLAGS(EnumName)                                                                                  \
    inline constexpr EnumName& operator|=(EnumName& left, EnumName right)                                              \
    {                                                                                                                  \
        return left = static_cast<EnumName>(static_cast<std::underlying_type_t<EnumName>>(left) |                      \
                                            static_cast<std::underlying_type_t<EnumName>>(right));                     \
    }                                                                                                                  \
    inline constexpr EnumName& operator&=(EnumName& left, EnumName right)                                              \
    {                                                                                                                  \
        return left = static_cast<EnumName>(static_cast<std::underlying_type_t<EnumName>>(left) &                      \
                                            static_cast<std::underlying_type_t<EnumName>>(right));                     \
    }                                                                                                                  \
    inline constexpr EnumName& operator^=(EnumName& left, EnumName right)                                              \
    {                                                                                                                  \
        return left = static_cast<EnumName>(static_cast<std::underlying_type_t<EnumName>>(left) ^                      \
                                            static_cast<std::underlying_type_t<EnumName>>(right));                     \
    }                                                                                                                  \
    inline constexpr EnumName operator|(EnumName left, EnumName right)                                                 \
    {                                                                                                                  \
        return static_cast<EnumName>(static_cast<std::underlying_type_t<EnumName>>(left) |                             \
                                     static_cast<std::underlying_type_t<EnumName>>(right));                            \
    }                                                                                                                  \
    inline constexpr EnumName operator&(EnumName left, EnumName right)                                                 \
    {                                                                                                                  \
        return static_cast<EnumName>(static_cast<std::underlying_type_t<EnumName>>(left) &                             \
                                     static_cast<std::underlying_type_t<EnumName>>(right));                            \
    }                                                                                                                  \
    inline constexpr EnumName operator^(EnumName left, EnumName right)                                                 \
    {                                                                                                                  \
        return static_cast<EnumName>(static_cast<std::underlying_type_t<EnumName>>(left) ^                             \
                                     static_cast<std::underlying_type_t<EnumName>>(right));                            \
    }                                                                                                                  \
    inline constexpr bool operator!(EnumName left)                                                                     \
    {                                                                                                                  \
        return !static_cast<std::underlying_type_t<EnumName>>(left);                                                   \
    }                                                                                                                  \
    inline constexpr EnumName operator~(EnumName left)                                                                 \
    {                                                                                                                  \
        return static_cast<EnumName>(~static_cast<std::underlying_type_t<EnumName>>(left));                            \
    }

VI_CORE_NS_BEGIN
template <typename TEnum>
inline constexpr bool testFlag(TEnum a, TEnum b)
{
    return static_cast<bool>(static_cast<std::underlying_type_t<TEnum>>(a & b));
}
VI_CORE_NS_END