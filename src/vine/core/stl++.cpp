#include <vine/core/String.h>
#include <vine/core/stl++.h>
#include <iostream>

VI_CORE_NS_BEGIN
std::ostream& operator<<(std::ostream& cout, const String& str) {

    cout << "-------std::cout << String called-------";
    return cout;
}
std::wostream& operator<<(std::wostream& wcout, const String& str) {

    wcout << "-------std::wcout << String called-------";
    return wcout;
}
VI_CORE_NS_END