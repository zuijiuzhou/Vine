#include <vine/appfw/ServiceManager.hpp>

#include <vine/Ptr.hpp>
#include <vine/di/Container.hpp>

V_APPFW_NS_BEGIN

struct ServiceManager::Data {
    RefPtr<di::Container> container;
};

ServiceManager::ServiceManager()
  : d(new Data())
{
    d->container = RefPtr<di::Container>(new di::Container());
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
