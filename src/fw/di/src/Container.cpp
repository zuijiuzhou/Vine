#include <vine/di/Container.hpp>

#include <vine/Exception.hpp>
#include <vine/Ptr.hpp>
#include <vine/di/Registration.hpp>

V_DI_NS_BEGIN

namespace
{

/**
 * @brief Checks whether a registration can ever produce a service instance.
 *
 * A registration is usable when it carries a pre-set instance, an instance
 * factory, or an impl type that can be resolved further.
 */
bool isValidRegistration(const Registration& reg)
{
    return reg.instance() != nullptr || static_cast<bool>(reg.instanceFactory()) || reg.impl() != nullptr;
}

} // namespace

V_OBJECT_META_IMPL(Container, RefObject)

struct Container::Impl {
    std::unordered_map<Type, Registration>      regs;       // Keyed by service type.
    std::unordered_map<Type, SPtr<RefObject>> singletons; // Lazily created singleton cache.
};

Container::Container()
  : d(new Impl)
{}

Container::~Container()
{
    delete d;
}

void Container::add(const Registration& reg)
{
    if (!isValidRegistration(reg)) {
        throw vine::Exception(vine::Exception::INVALID_ARGUMENTS, u8"The registration is invalid.");
    }
    auto type = reg.serviceType();
    if (d->regs.contains(type)) {
        throw vine::Exception(vine::Exception::ITEM_ALREADY_EXISTS, u8"The service is already registered.");
    }
    d->regs.emplace(type, reg);
}

RefObject* Container::resolve(Type type) const
{
    auto it = d->regs.find(type);
    if (it == d->regs.end())
        return nullptr;

    const Registration& reg = it->second;

    // A pre-set instance is always a singleton owned by the registration.
    if (RefObject* inst = reg.instance())
        return inst;

    // Return the cached singleton when it was created before.
    if (reg.lifetime() == Lifetime::Singleton) {
        auto cit = d->singletons.find(type);
        if (cit != d->singletons.end())
            return cit->second.get();
    }

    // The concrete creation target is the impl type when set, otherwise the service type.
    Type target = reg.impl() ? reg.impl() : type;

    RefObject* created = nullptr;
    if (auto fac = reg.instanceFactory()) {
        // The factory receives the container so it can resolve dependencies.
        created = fac(target, const_cast<Container&>(*this));
    }
    else if (target != type) {
        // Chained resolution reuses the impl type's own registration.
        created = resolve(target);
    }

    if (!created)
        return nullptr;

    // Cache singletons so later resolve calls return the same instance.
    if (reg.lifetime() == Lifetime::Singleton)
        d->singletons[type] = created;

    return created;
}

V_DI_NS_END
