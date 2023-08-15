#include <di/Registration.h>

VINE_DI_NS_BEGIN

struct Registration::Data
{
    ObjectPtr instance;
    Type implemented_by;
};

Registration::Registration(Type type)
    : d(new Data())
{

}

Registration *Registration::instance(const Object *inst)
{
    d->instance = inst;
    return this;
}

Registration *Registration::implementedBy(Type type){
    d->implemented_by = type;
    return this;
}

VINE_DI_NS_END