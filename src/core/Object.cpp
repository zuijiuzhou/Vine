#include <atomic>

#include <core/Object.h>
#include <core/Class.h>

ETD_CORE_NS_BEGIN

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
    static Class *cls = new Class(nullptr);
    return cls;
}

void Object::addRef()
{
    d->num_refs.fetch_add(1, std::memory_order_relaxed);
}

void Object::removeRef()
{
    d->num_refs.fetch_sub(1, std::memory_order_seq_cst);
}

ETD_CORE_NS_END