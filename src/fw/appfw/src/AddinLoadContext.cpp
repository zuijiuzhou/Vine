#include <vine/appfw/AddinLoadContext.hpp>

#include <vine/appfw/Application.hpp>
#include <vine/appfw/ConfigRegistry.hpp>

V_APPFW_NS_BEGIN

struct AddinLoadContext::Data {
    Application* app = nullptr;
};

AddinLoadContext::AddinLoadContext(Application* app)
    : d(new Data)
{
    d->app = app;
}

AddinLoadContext::~AddinLoadContext()
{
    delete d;
}

ConfigRegistry* AddinLoadContext::configs() const
{
    return d->app ? d->app->configRegistry() : nullptr;
}

V_APPFW_NS_END
