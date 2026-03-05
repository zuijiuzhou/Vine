#include <vine/String.hpp>

#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdarg>
#include <cstring>
#include <ctype.h>
#include <locale>
#include <vector>

#ifdef _WIN32
#    include <windows.h>
#endif

#include <vine/Exception.hpp>

VI_CORE_NS_BEGIN

// Specialization for char to use strlen
template <>
size_t cstrlen<char>(const char* data)
{
    return std::strlen(data);
}

// Specialization for char16_t
// On Windows wchar_t is 16-bit, on Linux/Unix it's typically 32-bit
template <>
size_t cstrlen<char16_t>(const char16_t* data)
{
#if defined(_WIN32) || defined(_WIN64)
    // Windows: wchar_t is 16-bit, safe to cast
    return std::wcslen(reinterpret_cast<const wchar_t*>(data));
#else
    // Linux/Unix: wchar_t is 32-bit, use generic version
    const char16_t* p = data;
    while (*p != 0) ++p;
    return p - data;
#endif
}

// Specialization for char32_t
// On Linux/Unix wchar_t is 32-bit, matching char32_t
template <>
size_t cstrlen<char32_t>(const char32_t* data)
{
#if !defined(_WIN32) && !defined(_WIN64)
    // Linux/Unix: wchar_t is 32-bit, safe to cast
    return std::wcslen(reinterpret_cast<const wchar_t*>(data));
#else
    // Windows: wchar_t is 16-bit, use generic version
    const char32_t* p = data;
    while (*p != 0) ++p;
    return p - data;
#endif
}

String String::toLower() const
{
    String result(*this);
    std::transform(result.stdstr_.begin(), result.stdstr_.end(), result.stdstr_.begin(), ::tolower);
    return result;
}

String String::toUpper() const
{
    String result(*this);
    std::transform(result.stdstr_.begin(), result.stdstr_.end(), result.stdstr_.begin(), ::toupper);
    return result;
}

String& String::trimStart()
{
    auto it = std::find_if_not(stdstr_.begin(), stdstr_.end(), ::isspace);
    stdstr_.erase(stdstr_.begin(), it);
    return *this;
}

String& String::trimEnd()
{
    auto it = std::find_if_not(stdstr_.rbegin(), stdstr_.rend(), ::isspace);
    stdstr_.erase(it.base(), stdstr_.end());
    return *this;
}

std::string String::toLocal8Bit() const
{

    return {};
}

std::u16string String::toUtf16() const
{
    if (stdstr_.empty())
        return {};

    return {};
}

std::u32string String::toUtf32() const
{
    return {};
}

String String::fromLocal8Bit(const char* data, size_type count)
{

    return {};
}

String String::fromUtf16(const char16_t* data, size_type count)
{
    if (!data || count == 0)
        return {};



    return {};
}

String String::fromUtf32(const char32_t* data, size_type count)
{
    return {};
}

VI_CORE_NS_END
