#include <core/Class.h>

ETD_CORE_NS_BEGIN

struct Class::Data
{
    const Class *parent = nullptr;
};

Class::Class(const Class *parent)
    : d_ptr(new Data)
{
    d_ptr->parent = parent;
}

const Class *Class::parent() const
{
    return d_ptr->parent;
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

ETD_CORE_NS_END
