#include <vine/appfw/ServiceManager.hpp>

#include <vine/Ptr.hpp>
#include <vine/di/Container.hpp>

V_APPFW_NS_BEGIN

struct ServiceManager::Impl {
    SPtr<di::Container> container;
};

ServiceManager::ServiceManager()
  : d(new Impl())
{
    d->container = SPtr<di::Container>(new di::Container());
}

ServiceManager::~ServiceManager()
{
    delete d;
}

ServiceManager* ServiceManager::registerService(const di::Registration& reg)
{
    d->container->add(reg);
    return this;
}

RefObject* ServiceManager::service(Type type) const
{
    return d->container->resolve(type);
}

V_APPFW_NS_END
