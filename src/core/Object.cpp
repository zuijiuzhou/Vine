#include <atomic>

#include <core/Object.h>
#include <core/Class.h>

VINE_CORE_NS_BEGIN

struct Object::Data
{
    std::atomic_uint num_refs = 0;
};

Object::Object()
    : d(new Data())
{
}

Object::~Object()
{
    delete d;
}

const Class *Object::isA() const
{
    return desc();
}

bool Object::isKindOf(const Class *type) const
{
    return isA()->isSubclassOf(type);
}

const Class *Object::desc()
{
    static Class *cls = new Class(nullptr, typeid(Object));
    return cls;
}

void Object::addRef()
{
    d->num_refs.fetch_add(1, std::memory_order_relaxed);
}

void Object::removeRef(bool del)
{
    d->num_refs.fetch_sub(1, std::memory_order_seq_cst);
    if (!d->num_refs && del)
        delete this;
}

VINE_CORE_NS_END