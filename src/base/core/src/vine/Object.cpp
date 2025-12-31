
#include <vine/Object.hpp>

#include <atomic>
#include <cstddef>

#include <vine/Class.hpp>
#include <vine/String.hpp>

VI_CORE_NS_BEGIN
Object::Object() noexcept
{}

Object::Object(const Object& other) noexcept
{}

Object::Object(Object&& other) noexcept
{}

Object::~Object() noexcept
{}

const Class* Object::getClass() const noexcept
{
    return desc();
}

bool Object::isKindOf(const Class* type) const
{
    return getClass()->isSubclassOf(type);
}

bool Object::equals(const Object& other) const noexcept
{
    return this == &other;
}

String Object::toString() const
{
    return getClass()->fullName();
}

const Class* Object::desc()
{
    static Class* cls = new Class(nullptr, typeid(Object));
    return cls;
}

VI_CORE_NS_END