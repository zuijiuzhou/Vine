#include <appfw/ServiceManager.h>

#include <di/Container.h>

VI_APPFW_NS_BEGIN
VI_OBJECT_META_IMPL(ServiceManager, Object)

struct ServiceManager::Data
{
    di::Container* container = nullptr;
};

ServiceManager::ServiceManager()
    : d(new Data())
{
    d->container = new di::Container();
    d->container->addRef();
}

ServiceManager::~ServiceManager(){
    d->container->removeRef();
    delete d;
}

ServiceManager* ServiceManager::registerService(di::Registration* reg){

    return this;
}

Object* ServiceManager::getService(Type type) const{
    return d->container->resolve(type);
}

VI_APPFW_NS_END