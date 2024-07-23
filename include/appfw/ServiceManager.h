#pragma once
#include "appfw_global.h"

#include "di/Registration.h"

VI_APPFW_NS_BEGIN

class VI_APPFW_API ServiceManager : public Object {
    VI_OBJECT_META;
    VI_DISABLE_COPY_MOVE(ServiceManager);

  public:
    ServiceManager();
    ~ServiceManager();

  public:
    ServiceManager* registerService(di::Registration* reg);
    Object*         getService(Type type) const;

  private:
    VI_OBJECT_DATA
};
using ServiceManagerPtr = RefPtr<ServiceManager>;

VI_APPFW_NS_END