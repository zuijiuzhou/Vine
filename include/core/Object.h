#pragma once
#include "core_global.h"

#include <concepts>
#include <type_traits>
#include <typeinfo>

#include "core_defs.h"
#include "Class.h"
#include "Ptr.h"

VI_CORE_NS_BEGIN

class String;
class Object;

template <typename T>
concept Objectifiable = std::is_base_of<Object, T>::value;

template <typename T>
concept Cloneable = requires(T t) {
    {
        t.clone()
    } -> std::same_as<T>;
};

template <typename T>
concept Stringable = requires(T t) {
    {
        t.toString()
    } -> std::same_as<String>;
};

template <typename T>
concept Comparable = requires(T t, const Object *obj) {
    {
        t.compareTo(obj)
    } -> std::same_as<int>;
};

template <typename T>
concept Enumerable = requires(T t, UInt64 i) {
    {
        t.count()
    } -> std::same_as<UInt64>;
    {
        t.at(i)
    };
};

class VI_CORE_API Object
{
public:
    Object();
    virtual ~Object();

public:
    virtual const Class *getType() const;

    bool isKindOf(const Class *type) const;

    template <Objectifiable T>
    bool isKindOf()
    {
        return isKindOf(T::desc());
    }

    template <Objectifiable T>
    T *cast()
    {
        if (isKindOf(T::desc()))
            return static_cast<T *>(this);
        return nullptr;
    }

    template <Objectifiable T>
    const T *cast() const
    {
        if (isKindOf(T::desc()))
            return static_cast<const T *>(this);
        return nullptr;
    }

    void addRef();

    void removeRef(bool del = true);

    UInt64 numRefs() const noexcept;

    virtual bool equals(const Object &other) const;

public:
    static const Class *desc();

    template <Objectifiable T>
    static T *cast(T *obj)
    {
        if (obj)
            return obj->cast<T>();
        return nullptr;
    }

    template <Objectifiable T>
    static const T *cast(const T *obj)
    {
        if (obj)
            return obj->cast<T>();
        return nullptr;
    }

    template <Objectifiable T>
    static T &cast(Object &obj)
    {
        if (obj.getType() == T::desc())
            return static_cast<T &>(obj);
        throw std::exception();
    }

    template <Objectifiable T>
    static const T &cast(const T &obj)
    {
        if (obj.getType() == T::desc())
            return static_cast<const T &>(obj);
        throw std::exception();
    }

private:
    VI_DISABLE_COPY_MOVE(Object);

private:
    struct Data;
    Data *d;
};

using ObjectPtr = RefPtr<Object>;

VI_CORE_NS_END


#define VI_OBJECT_META                                                    \
public:                                                                   \
    virtual const Class *getType() const override;                        \
    static const Class *desc();

#define VI_OBJECT_META_IMPL(Sub, Parent)                                  \
    const Class *Sub::getType() const                                     \
    {                                                                     \
        return desc();                                                    \
    }                                                                     \
                                                                          \
    const Class *Sub::desc()                                              \
    {                                                                     \
        static const Class *cls = new Class(Parent::desc(), typeid(Sub)); \
        return cls;                                                       \
    }

#define VI_TMPL_OBJECT_META(Sub, Parent)                                  \
    const Class *getType() const override                                 \
    {                                                                     \
        return desc();                                                    \
    }                                                                     \
                                                                          \
    static const Class *desc()                                            \
    {                                                                     \
        static const Class *cls = new Class(Parent::desc(), typeid(Sub)); \
        return cls;                                                       \
    }

#define VI_OBJECT_DATA \
    struct Data;       \
    Data *const d;

// #define VI_OBJ(TParent) \
// static const Class* desc(); \
// virtual const Class* getType() const override;
