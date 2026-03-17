#pragma once
#include "appfw_global.hpp"

#include <vine/RefObject.hpp>
#include <vine/di/Registration.hpp>

V_APPFW_NS_BEGIN

class V_APPFW_API ServiceManager : public RefObject {
    V_OBJECT_META_DECL;

  public:
    ServiceManager();
    ~ServiceManager();

  public:
    ServiceManager* registerService(di::Registration* reg);
    RefObject*      getService(Type type) const;

  private:
    struct Data;
    Data* const d;
};

using ServiceManagerPtr = RefPtr<ServiceManager>;

V_APPFW_NS_END
