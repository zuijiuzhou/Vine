
#include <di/Registration.h>

#include <type_traits>

#include <core/Ptr.h>

VI_DI_NS_BEGIN

VI_OBJECT_META_IMPL(Registration, Object)

struct Registration::Data {
    Type            service_type      = nullptr;
    Type            service_impl_type = nullptr;
    ObjectPtr       inst;
    InstanceFactory inst_fac;
    Lifetime        lifetime = Lifetime::Transient;
};

Registration::Registration(Type type)
  : d(new Data()) {
    d->service_type = type;
}

Registration::Registration(const Registration& reg)
  : d(new Data()) {
    *d = *reg.d;
}

Registration* Registration::instance(Object* inst) {
    d->inst     = inst;
    d->lifetime = Lifetime::Singleton;
    return this;
}

Registration* Registration::impl(Type type) {
    d->service_impl_type = type;
    return this;
}

Registration* Registration::lifetime(Lifetime lt) {
    d->lifetime = lt;
    return this;
}

Registration* Registration::useFactory(InstanceFactory fac) {
    d->inst_fac = std::move(fac);
    return this;
}

Lifetime Registration::lifetime() const {
    return d->lifetime;
}

Registration& Registration::operator=(const Registration& reg) {
    Object::operator=(reg);
    *d = *reg.d;
    return *this;
}

VI_DI_NS_END