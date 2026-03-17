#pragma once
#include "runtime_global.hpp"

#include <vine/String.hpp>

V_RUNTIME_NS_BEGIN

class V_RUNTIME_API CacheManager {
  public:
    virtual void set(const String& key, void* value, size_t timeout) = 0;
};

V_RUNTIME_NS_END
