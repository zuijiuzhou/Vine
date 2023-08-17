#pragma once
#include "core_global.h"

#include <type_traits>

#include "Std.h"
#include "Ptr.h"

VINE_CORE_NS_BEGIN

class String;
class Class;
class Object;

template <typename T>
concept Objectifiable = std::is_base_of<Object, T>::value;

template <typename T>
concept Cloneable = requires(T t) 
{
    { t.clone() } -> std::same_as<T>;
};

template <typename T>
concept Stringable = requires(T t) 
{
    { t.toString() } -> std::same_as<String>; 
};

template <typename T>
concept Comparable = requires(T t, const Object* obj) 
{
    { t.compareTo(obj) } -> std::same_as<int>; 
};

template<typename T>
concept Enumerable = requires(T t, ULong i)
{
    { t.count() } -> std::same_as<ULong>;
    { t.at(i) };
};

class VINE_CORE_API Object
{
public:
    Object();
    virtual ~Object();

public:
    virtual const Class *isA() const;

    bool isKindOf(const Class *type) const;

    template <Objectifiable T>
    bool isKindOf()
    {
        return isKindOf(T::desc());
    }

    template <Objectifiable T>
    T *as()
    {
        if (isKindOf(T::desc()))
            return static_cast<T *>(this);
        return nullptr;
    }

    template <Objectifiable T>
    const T *as() const
    {
        if (isKindOf(T::desc()))
            return static_cast<const T *>(this);
        return nullptr;
    }

    void addRef();

    void removeRef(bool del = true);

    UInt numRefs() const;

    virtual bool equals(const Object& other) const;

public:
    static const Class *desc();

private:
    struct Data;
    Data *d;
};

using ObjectPtr = RefPtr<Object>;

VINE_CORE_NS_END

// #define VI_OBJ(TParent) \
// static const Class* desc(); \
// virtual const Class* isA() const override;
