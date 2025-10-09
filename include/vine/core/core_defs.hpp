#pragma once

#include "core_global.hpp"

#include <type_traits>

#define VI_DISABLE_COPY(_TCls)                                                                                         \
  private:                                                                                                             \
    _TCls(const _TCls&)            = delete;                                                                           \
    _TCls& operator=(const _TCls&) = delete;

#define VI_DISABLE_MOVE(_TCls)                                                                                         \
  private:                                                                                                             \
    _TCls(_TCls&&)            = delete;                                                                                \
    _TCls& operator=(_TCls&&) = delete;

#define VI_DISABLE_COPY_MOVE(_TCls)                                                                                    \
    VI_DISABLE_COPY(_TCls)                                                                                             \
    VI_DISABLE_MOVE(_TCls)

#define VI_ENUM_CLASS_FLAGS(_TEnum)                                                                                    \
    inline constexpr _TEnum& operator|=(_TEnum& x, _TEnum y) {                                                         \
        return x = static_cast<_TEnum>(static_cast<std::underlying_type_t<_TEnum>>(x) |                                \
                                       static_cast<std::underlying_type_t<_TEnum>>(y));                                \
    }                                                                                                                  \
    inline constexpr _TEnum& operator&=(_TEnum& x, _TEnum y) {                                                         \
        return x = static_cast<_TEnum>(static_cast<std::underlying_type_t<_TEnum>>(x) &                                \
                                       static_cast<std::underlying_type_t<_TEnum>>(y));                                \
    }                                                                                                                  \
    inline constexpr _TEnum& operator^=(_TEnum& x, _TEnum y) {                                                         \
        return x = static_cast<_TEnum>(static_cast<std::underlying_type_t<_TEnum>>(x) ^                                \
                                       static_cast<std::underlying_type_t<_TEnum>>(y));                                \
    }                                                                                                                  \
    inline constexpr _TEnum operator|(_TEnum x, _TEnum y) {                                                            \
        return static_cast<_TEnum>(static_cast<std::underlying_type_t<_TEnum>>(x) |                                    \
                                   static_cast<std::underlying_type_t<_TEnum>>(y));                                    \
    }                                                                                                                  \
    inline constexpr _TEnum operator&(_TEnum x, _TEnum y) {                                                            \
        return static_cast<_TEnum>(static_cast<std::underlying_type_t<_TEnum>>(x) &                                    \
                                   static_cast<std::underlying_type_t<_TEnum>>(y));                                    \
    }                                                                                                                  \
    inline constexpr _TEnum operator^(_TEnum x, _TEnum y) {                                                            \
        return static_cast<_TEnum>(static_cast<std::underlying_type_t<_TEnum>>(x) ^                                    \
                                   static_cast<std::underlying_type_t<_TEnum>>(y));                                    \
    }                                                                                                                  \
    inline constexpr bool operator!(_TEnum x) {                                                                        \
        return !static_cast<std::underlying_type_t<_TEnum>>(x);                                                        \
    }                                                                                                                  \
    inline constexpr _TEnum operator~(_TEnum x) {                                                                      \
        return static_cast<_TEnum>(~static_cast<std::underlying_type_t<_TEnum>>(x));                                   \
    }

VI_CORE_NS_BEGIN
template <typename _TEnum> inline constexpr bool testFlag(_TEnum a, _TEnum b) {
    return static_cast<bool>(static_cast<std::underlying_type_t<_TEnum>>(a & b));
}
VI_CORE_NS_END