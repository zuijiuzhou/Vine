#include <vine/di/Container.hpp>

#include <vine/Exception.hpp>
#include <vine/di/Registration.hpp>

V_DI_NS_BEGIN

namespace
{

bool isValidRegistration(const Registration& reg)
{ return true; }

} // namespace

V_OBJECT_META_IMPL(Container, RefObject)

struct Container::Impl {
    std::unordered_map<Type, Registration> regs;
};

Container::Container()
  : d(new Impl)
{}

void Container::add(const Registration& reg)
{
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
    if (d->regs.contains(type)) {
        auto& reg  = d->regs.at(type);
        auto  inst = reg.instance();
        if (inst)
            return inst;

        auto impl_type = reg.serviceType();
        if (impl_type) {}

        auto fac = reg.instanceFactory();
        if (fac) { inst = fac(reg.serviceType(), const_cast<Container&>(*this)); }
        return inst;
    }
    return nullptr;
}

V_DI_NS_END
