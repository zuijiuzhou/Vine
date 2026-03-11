#pragma once
#include "runtime_global.hpp"

#include <vine/String.hpp>

VI_RUNTIME_NS_BEGIN

class VI_RUNTIME_API CacheManager {
  public:
    virtual void set(const String& key, void* value, size_t timeout) = 0;
};

VI_RUNTIME_NS_END
