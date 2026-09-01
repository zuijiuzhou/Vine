#pragma once

// Forwarding shim for the deprecated legacy SPtr/WPtr family.
// Prefer intrusive_ptr<T> (and RefCounted<T>); see Ptr.hpp in deprecated/ for
// details.
#include <vine/deprecated/Ptr.hpp>
