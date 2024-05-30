#include <iostream>
#include "String.h"

VI_CORE_NS_BEGIN

VI_CORE_API std::ostream &operator<<(std::ostream &cout, const String &str);
VI_CORE_API std::wostream &operator<<(std::wostream &wcout, const String &str);

VI_CORE_NS_END