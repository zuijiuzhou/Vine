#pragma once
#include "core_global.h"

#include <concepts>
#include <type_traits>
#include <typeinfo>

#include "Class.h"
#include "Ptr.h"
#include "Std.h"
#include "String.h"
#include "core_defs.h"

VI_CORE_NS_BEGIN

class Object;

//template <typename T>
//    requires std::is_base_of<Object, T>::value
//class RefPtr;

template <typename T>
concept Objectifiable = std::is_base_of<Object, T>::value;

template <typename T>
concept Cloneable = requires(T t) {
    { t.clone() } -> std::same_as<T>;
};

template <typename T>
concept Stringable = requires(T t) {
    { t.toString() } -> std::same_as<String>;
};

template <typename T>
concept Comparable = requires(T t, const Object* obj) {
    { t.compareTo(obj) } -> std::same_as<int>;
};

class VI_CORE_API Object {
  public:
    Object() noexcept;
    Object(const Object& other) noexcept;
    Object(Object&& other) noexcept;
    virtual ~Object() noexcept;

  public:
    virtual const Class* getClass() const noexcept;

    bool isKindOf(const Class* type) const;

    template <Objectifiable T> bool isKindOf() { return isKindOf(T::desc()); }

    template <Objectifiable T> T* cast() {
        if (isKindOf(T::desc())) return static_cast<T*>(this);
        return nullptr;
    }

    template <Objectifiable T> const T* cast() const {
        if (isKindOf(T::desc())) return static_cast<const T*>(this);
        return nullptr;
    }

    void addRef();

    void removeRef(bool del = true);

    UInt64 getRefs() const noexcept;

    virtual bool equals(const Object& other) const noexcept;

    virtual String toString() const;

  public:
    Object& operator=(const Object& right) noexcept;
    Object& operator=(Object&& right) noexcept;

  public:
    static const Class* desc();

    template <Objectifiable T> static T* cast(T* obj) {
        if (obj) return obj->cast<T>();
        return nullptr;
    }

    template <Objectifiable T> static const T* cast(const T* obj) {
        if (obj) return obj->cast<T>();
        return nullptr;
    }

    template <Objectifiable T> static T& cast(Object& obj) {
        if (obj.getClass() == T::desc()) return static_cast<T&>(obj);
        throw std::exception();
    }

    template <Objectifiable T> static const T& cast(const T& obj) {
        if (obj.getClass() == T::desc()) return static_cast<const T&>(obj);
        throw std::exception();
    }

  private:
    struct Data;
    Data* d;
};

using ObjectPtr = RefPtr<Object>;

VI_CORE_NS_END

#define VI_OBJECT_META                                                                                                 \
  public:                                                                                                              \
    virtual const vine::Class* getClass() const noexcept override;                                                     \
    static const vine::Class*  desc();

#define VI_OBJECT_META_IMPL(Sub, Parent)                                                                               \
    const vine::Class* Sub::getClass() const noexcept {                                                                \
        return desc();                                                                                                 \
    }                                                                                                                  \
                                                                                                                       \
    const vine::Class* Sub::desc() {                                                                                   \
        static const vine::Class* cls = new vine::Class(Parent::desc(), typeid(Sub));                                  \
        return cls;                                                                                                    \
    }

#define VI_TMPL_OBJECT_META(Sub, Parent)                                                                               \
  public:                                                                                                              \
    const vine::Class* getClass() const noexcept override {                                                            \
        return desc();                                                                                                 \
    }                                                                                                                  \
                                                                                                                       \
    static const vine::Class* desc() {                                                                                 \
        static const vine::Class* cls = new vine::Class(Parent::desc(), typeid(Sub));                                  \
        return cls;                                                                                                    \
    }

#define VI_OBJECT_DATA                                                                                                 \
    struct Data;                                                                                                       \
    Data* const d;

// #define VI_OBJ(TParent) \
// static const Class* desc(); \
// virtual const Class* getType() const override;
