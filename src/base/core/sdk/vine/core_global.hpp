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

#define VI_DECLARE_PIMPL_CLASS(ClassName)                                                                              \
    class ClassName;                                                                                                   \
    class ClassName##Private;

#define VI_DECLARE_DPTR(ClassName)                                                                                     \
  protected:                                                                                                             \
    ClassName##Private* const d_ptr;

#define VI_DECLARE_VPTR(ClassName)                                                                                     \
  protected:                                                                                                             \
    ClassName* v_ptr;

#define VI_DECLARE_PRIVATE(ClassName)                                                                                  \
  public:                                                                                                              \
    friend class ClassName##Private;                                                                                   \
    inline ClassName##Private* getDPtr()                                                                               \
    {                                                                                                                  \
        return reinterpret_cast<ClassName##Private*>(d_ptr);                                                           \
    }                                                                                                                  \
    inline const ClassName##Private* getDPtr() const                                                                   \
    {                                                                                                                  \
        return reinterpret_cast<const ClassName##Private*>(d_ptr);                                                     \
    }

// #define VI_DECLARE_CTOR_PRIVATE(ClassName)                                                                             \
//   public:                                                                                                              \
//     ClassName##Private(ClassName* vptr);

// #define VI_DECLARE_DTOR_PUBLIC(ClassName)                                                                              \
//   public:                                                                                                              \
//     virtual ~ClassName();

#define VI_DECLARE_PUBLIC(ClassName)                                                                                   \
  public:                                                                                                              \
    friend class ClassName;                                                                                            \
    inline ClassName* getVPtr()                                                                                        \
    {                                                                                                                  \
        return reinterpret_cast<ClassName*>(v_ptr);                                                                    \
    }                                                                                                                  \
    inline const ClassName* getVPtr() const                                                                            \
    {                                                                                                                  \
        return reinterpret_cast<const ClassName*>(v_ptr);                                                              \
    }

// pimpl模式中，将d_ptr转换为派生类的具体IMPL类型指针，类似于Qt中的Q_D宏
#define VI_D(ClassName) auto* const d = getDPtr();
// pimpl模式中，将v_ptr转换为派生类具体类型指针，类似于Qt中的Q_Q宏
#define VI_V(ClassName) auto* const v = getVPtr();

// 定义类的共享指针和弱指针类型，并前向声明类
 // #define VI_DEFINE_PTR(ClassName) \
//     class ClassName; \
//     using ClassName##SharedPtr = std::shared_ptr<ClassName>; \
// using ClassName##WeakPtr   = std::weak_ptr<ClassName>;
