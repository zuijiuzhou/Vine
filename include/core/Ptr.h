#pragma once

#include <memory>

ETD_CORE_NS_BEGIN
template <typename T>
using SharedPtr = std::shared_ptr<T>;
ETD_CORE_NS_END
