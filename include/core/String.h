#pragma once
#include "core_global.h"

#include <string>

VINE_CORE_NS_BEGIN
// using String = std::u32string;

using Char = char32_t;

class VINE_CORE_API String{
public:
    String();
    String(const Char* data);
    String(const String& other);
    String(String&& other);
    virtual ~String();

public:
    String substr(int start) const;

    String substr(int start, int count) const;

    size_t size() const;

    size_t length() const;

    void clear();

    bool empty() const;

    const Char* data() const;

    void set(const Char* data);

    void set(const Char* data, size_t len);

public:
    static String fromUtf8(const char* data);

    static String fromUtf16(const char16_t* data);

    static String fromLocal8Bit(const char* data);

    static String fromStdString(const std::string& data);

    static String fromStdWString(const std::wstring& data);

public:
    String& operator =(const String& right);

private:
    Char* data_ = nullptr;
    size_t len_ = 0;
};
VINE_CORE_NS_END