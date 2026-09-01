#pragma once

// Forwarding shim for the deprecated legacy reference-counting base class.
// Prefer RefCounted<Derived> + intrusive_ptr<Derived>; see RefObject.hpp in
// deprecated/ for details.
#include <vine/deprecated/RefObject.hpp>
