#pragma once

#include "core_global.hpp"

V_CORE_NS_BEGIN

/**
 * @brief Raw, non-owning pointer alias.
 *
 * RawPtr<T> is a plain T*. It denotes a non-owning reference to an object whose
 * lifetime is managed elsewhere. It never participates in reference counting
 * and never deletes the pointee.
 */
template <typename T>
using RawPtr = T*;

V_CORE_NS_END
