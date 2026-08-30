#pragma once

// Forwarding shim for the deprecated legacy reference-counting base class.
// Prefer RefCounted<Derived> + IntrusivePtr<Derived>; see RefObject.hpp in
// deprecated/ for details.
#include <vine/deprecated/RefObject.hpp>
