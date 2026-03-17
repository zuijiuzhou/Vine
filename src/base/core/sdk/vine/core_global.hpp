#pragma once

#include <vine/vi_global.hpp>

#ifdef V_CORE_LIB
#    define V_CORE_API __API_EXPORT__
#else
#    define V_CORE_API __API_IMPORT__
#endif

#define V_CORE_NS_BEGIN V_ROOT_NS_BEGIN

#define V_CORE_NS_END V_ROOT_NS_END

#define V_DECLARE_PIMPL(ClassName)                                                                                                                             \
    class ClassName;                                                                                                                                           \
    class ClassName##Private;

#define V_DECLARE_DPTR(ClassName)                                                                                                                              \
  protected:                                                                                                                                                   \
    ClassName##Private* const d_ptr;

#define V_DECLARE_VPTR(ClassName)                                                                                                                              \
  protected:                                                                                                                                                   \
    ClassName* v_ptr;

#define V_DECLARE_PRIVATE(ClassName)                                                                                                                           \
  public:                                                                                                                                                      \
    friend class ClassName##Private;                                                                                                                           \
    inline ClassName##Private* getDPtr()                                                                                                                       \
    {                                                                                                                                                          \
        return reinterpret_cast<ClassName##Private*>(d_ptr);                                                                                                   \
    }                                                                                                                                                          \
    inline const ClassName##Private* getDPtr() const                                                                                                           \
    {                                                                                                                                                          \
        return reinterpret_cast<const ClassName##Private*>(d_ptr);                                                                                             \
    }

// #define V_DECLARE_CTOR_PRIVATE(ClassName)                                                                             \
//   public:                                                                                                              \
//     ClassName##Private(ClassName* vptr);

// #define V_DECLARE_DTOR_PUBLIC(ClassName)                                                                              \
//   public:                                                                                                              \
//     virtual ~ClassName();

#define V_DECLARE_PUBLIC(ClassName)                                                                                                                            \
  public:                                                                                                                                                      \
    friend class ClassName;                                                                                                                                    \
    inline ClassName* getVPtr()                                                                                                                                \
    {                                                                                                                                                          \
        return reinterpret_cast<ClassName*>(v_ptr);                                                                                                            \
    }                                                                                                                                                          \
    inline const ClassName* getVPtr() const                                                                                                                    \
    {                                                                                                                                                          \
        return reinterpret_cast<const ClassName*>(v_ptr);                                                                                                      \
    }

// pimpl模式中，将d_ptr转换为派生类的具体IMPL类型指针，类似于Qt中的Q_D宏
#define V_D(ClassName) auto* const d = getDPtr();
// pimpl模式中，将v_ptr转换为派生类具体类型指针，类似于Qt中的Q_Q宏
#define V_V(ClassName) auto* const v = getVPtr();

// 定义类的共享指针和弱指针类型，并前向声明类
#define V_DEFINE_PTR(ClassName)                                                                                                                                \
    class ClassName;                                                                                                                                           \
    using ClassName##SharedPtr = RefPtr<ClassName>;                                                                                                            \
    // using ClassName##WeakPtr   = std::weak_ptr<ClassName>;
