#pragma once
#include "runtime_global.h"

#include <vine/core/String.h"

VI_RUNTIME_NS_BEGIN

class VI_RUNTIME_API CacheManager
{
public:
    void set(const String &key, void *value, size_t timeout) = 0;
};

VI_RUNTIME_NS_END