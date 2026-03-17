#include <vine/appfw/ServiceManager.hpp>

#include <vine/di/Container.hpp>

V_APPFW_NS_BEGIN
V_OBJECT_META_IMPL(ServiceManager, RefObject)

struct ServiceManager::Data {
    di::Container* container = nullptr;
};

ServiceManager::ServiceManager()
  : d(new Data())
{
    d->container = new di::Container();
    d->container->strong_ref();
}

ServiceManager::~ServiceManager()
{
    d->container->strong_unref();
    delete d;
}

ServiceManager* ServiceManager::registerService(di::Registration* reg)
{

    return this;
}

RefObject* ServiceManager::getService(Type type) const
{
    return d->container->resolve(type);
}

V_APPFW_NS_END
