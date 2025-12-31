
#include <type_traits>

#include <vine/Exception.hpp>
#include <vine/Ptr.hpp>
#include <vine/di/Registration.hpp>

#include "vine/di/Registration_p.hpp"

VI_DI_NS_BEGIN

VI_OBJECT_META_IMPL(Registration, RefObject)

Registration::Registration(Type type)
  : d(new RegistrationPrivate()) {
    VI_CHECK_NULL_THROW(type);
    d->service_type = type;
}

Registration::Registration(const Registration& reg)
  : d(new RegistrationPrivate()) {
    *d = *reg.d;
}

Registration* Registration::instance(RefObject* inst) {
    if (inst && !inst->isKindOf(d->service_type)) {
        throw vine::Exception(vine::Exception::INVALID_ARGUMENTS, U"The 'inst' is not kind of the service type.");
    }
    d->inst     = inst;
    d->lifetime = Lifetime::Singleton;
    return this;
}

RefObject* Registration::instance() const {
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
    *d = *reg.d;
    return *this;
}

VI_DI_NS_END