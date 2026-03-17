#include "core_global.hpp"

#include <iosfwd>

V_CORE_NS_BEGIN

class String;

V_CORE_API std::ostream& operator<<(std::ostream& cout, const String& str);

V_CORE_NS_END
