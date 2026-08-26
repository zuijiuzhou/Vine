#pragma once
#include "appfw_global.hpp"

#include <vine/RefObject.hpp>
#include <vine/di/Registration.hpp>

V_APPFW_NS_BEGIN

class V_APPFW_API ServiceManager {

  public:
    ServiceManager();
    ~ServiceManager();

  public:
    ServiceManager* registerService(const di::Registration& reg);
    RefObject*      service(Type type) const;

  private:
    struct Data;
    Data* const d;
};

V_APPFW_NS_END
