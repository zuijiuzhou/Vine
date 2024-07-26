#include <map>

#include <vine/core/Exception.h>
#include <vine/di/Container.h>
#include <vine/di/Registration.h>

#include "Registration_p.h"

VI_DI_NS_BEGIN

namespace {
bool isValidRegistration(Registration* r) {
    return true;
}
} // namespace
VI_OBJECT_META_IMPL(Container, Object)

struct Container::Data {
    std::map<Type, Registration*> regs;
};

Container::Container()
  : d(new Data()) {
}

void Container::add(Registration* reg) {
    if (isValidRegistration(reg)) {
        auto type     = reg->serviceType();
        d->regs[type] = reg;
    }
    else {
        throw vine::Exception(vine::Exception::INVALID_ARGUMENTS, U"The registration is invalid.");
    }
}

Object* Container::resolve(Type type) const {
    if (d->regs.contains(type)) {
        auto reg = d->regs[type];

        auto reg_p = reg->d;

        auto inst = reg_p->inst.get();
        if (inst) return inst;

        auto impl_type = reg_p->service_impl_type;
        if (impl_type) {
            
        }

        auto fac = reg->instanceFactory();
        if (fac) {
            inst = fac(reg->serviceType(), const_cast<Container*>(this));
        }
    }
    return nullptr;
}

VI_DI_NS_END