#include <core/Class.h>

VINE_CORE_NS_BEGIN

struct Class::Data
{
    const Class *parent = nullptr;
};

Class::Class(const Class *parent)
    : d(new Data)
{
    d->parent = parent;
}

const Class *Class::parent() const
{
    return d->parent;
}

bool Class::isSubclassOf(const Class *cls) const
{
    if (cls == nullptr)
        return false;

    auto type = this;
    do
    {
        if (type == cls)
            return true;
        type = type->parent();
    } while (type);
    return false;
}

VINE_CORE_NS_END
