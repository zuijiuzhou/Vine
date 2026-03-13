#include <vine/di/Container.hpp>

#include <vine/Exception.hpp>
#include <vine/di/ContainerPrivate.hpp>
#include <vine/di/Registration.hpp>

VI_DI_NS_BEGIN

namespace
{

bool isValidRegistration(const Registration& reg)
{
    return true;
}

} // namespace

ContainerPrivate::ContainerPrivate()
{}

VI_OBJECT_META_IMPL(Container, RefObject)

Container::Container()
  : RefObject(new ContainerPrivate())
{}

void Container::add(const Registration& reg)
{
    VI_D(Container);

    if (isValidRegistration(reg)) {
        auto type = reg.serviceType();
        d->regs.insert({ type, reg });
    }
    else {
        throw vine::Exception(vine::Exception::INVALID_ARGUMENTS, u8"The registration is invalid.");
    }
}

RefObject* Container::resolve(Type type) const
{
    VI_D(Container);

    if (d->regs.contains(type)) {
        auto& reg = d->regs.at(type);

        auto inst = reg.instance();
        if (inst)
            return inst;

        auto impl_type = reg.serviceType();
        if (impl_type) {
        }

        auto fac = reg.instanceFactory();
        if (fac) {
            inst = fac(reg.serviceType(), const_cast<Container&>(*this));
        }
    }
    return nullptr;
}

VI_DI_NS_END
