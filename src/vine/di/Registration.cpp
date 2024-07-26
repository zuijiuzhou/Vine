
#include <type_traits>

#include <vine/core/Exception.h>
#include <vine/core/Ptr.h>
#include <vine/di/Registration.h>

#include "Registration_p.h"

VI_DI_NS_BEGIN

VI_OBJECT_META_IMPL(Registration, Object)

Registration::Registration(Type type)
  : d(new RegistrationPrivate()) {
    VI_CHECK_NULL(type);
    d->service_type = type;
}

Registration::Registration(const Registration& reg)
  : d(new RegistrationPrivate()) {
    *d = *reg.d;
}

Registration* Registration::instance(Object* inst) {
    if (inst && !inst->isKindOf(d->service_type)) {
        throw vine::Exception(vine::Exception::INVALID_ARGUMENTS, U"The inst is not kind of the service type.");
    }
    d->inst     = inst;
    d->lifetime = Lifetime::Singleton;
    return this;
}

Object* Registration::instance() const {
    return d->inst.get();
}

Registration* Registration::impl(Type type) {
    d->service_impl_type = type;
    return this;
}

Type Registration::impl() {
    return d->service_impl_type;
}

Registration* Registration::lifetime(Lifetime lt) {
    d->lifetime = lt;
    return this;
}

Lifetime Registration::lifetime() const {
    return d->lifetime;
}

Type Registration::serviceType() const {
    return d->service_type;
}

Registration* Registration::instanceFactory(InstanceFactory fac) {
    d->inst_fac = std::move(fac);
    return this;
}

InstanceFactory Registration::instanceFactory() const {
    return d->inst_fac;
}

Registration& Registration::operator=(const Registration& reg) {
    Object::operator=(reg);
    *d = *reg.d;
    return *this;
}

VI_DI_NS_END