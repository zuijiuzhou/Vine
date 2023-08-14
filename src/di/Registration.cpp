#include <di/Registration.h>

VINE_DI_NS_BEGIN

struct Registration::Data
{
    Object::Ptr obj;
};

Registration::Registration()
    : d(new Data())
{

}

Registration *Registration::instance(const Object *obj)
{
    // d->obj = Object::Ptr(obj);
    return this;
}

VINE_DI_NS_END