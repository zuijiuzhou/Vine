
#include <vine/Object.hpp>

#include <vine/Class.hpp>
#include <vine/String.hpp>

V_CORE_NS_BEGIN

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
    static Class* cls = new Class(typeid(Object), nullptr);
    return cls;
}

V_CORE_NS_END
