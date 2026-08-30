#pragma once
#include "appfw_global.hpp"

#include <memory>

#include <vine/RawPtr.hpp>
#include <vine/di/Registration.hpp>
#include <vine/di/ServiceBase.hpp>

V_APPFW_NS_BEGIN

class V_APPFW_API ServiceManager {

  public:
    ServiceManager();
    ~ServiceManager();

  public:
    ServiceManager*   registerService(const di::Registration& reg);
    RawPtr<vine::di::ServiceBase> service(TypeId type) const;

  private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

V_APPFW_NS_END
