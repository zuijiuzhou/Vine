#pragma once

#include <memory>

ETD_CORE_NS_BEGIN
template <typename T>
using Ptr = std::shared_ptr<T>;
ETD_CORE_NS_END
