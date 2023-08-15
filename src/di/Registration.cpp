#include <di/Registration.h>

VINE_DI_NS_BEGIN

struct Registration::Data
{
    Type service_type = nullptr;
    ;
    Type service_impl_type = nullptr;
    ObjectPtr instance;
    Lifetime lifetime = Lifetime::Transient;
};

Registration::Registration(Type type)
    : d(new Data())
{
    d->service_type = type;
}

Registration *Registration::instance(Object *inst)
{
    d->instance = inst;
    d->lifetime = Lifetime::Singleton;
    return this;
}

Registration *Registration::impl(Type type)
{
    d->service_impl_type = type;
    return this;
}

Registration *Registration::lifetime(Lifetime lt)
{
    d->lifetime = lt;
    return this;
}

Lifetime Registration::lifetime() const
{
    return d->lifetime;
}

VINE_DI_NS_END