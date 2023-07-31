#include <core/Object.h>
#include <core/Class.h>

ETD_CORE_NS_BEGIN

struct Object::Data
{
};

Object::~Object(){
    delete d_ptr;
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

ETD_CORE_NS_END