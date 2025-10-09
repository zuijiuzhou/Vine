#include <iosfwd>

VI_CORE_NS_BEGIN

class String;

VI_CORE_API std::ostream& operator<<(std::ostream& cout, const String& str);
VI_CORE_API std::wostream& operator<<(std::wostream& wcout, const String& str);

VI_CORE_NS_END