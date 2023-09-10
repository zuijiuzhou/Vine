#pragma once
#include "core_global.h"

#include <iostream>

#include "Std.h"


VI_CORE_NS_BEGIN
// using String = std::u32string;

/*
字符串，内部UTF-32编码，字节序小端
*/
class VI_CORE_API String{
public:
    using iterator = Char*;
    using const_iterator = const Char*;
    using resverse_iterator = std::reverse_iterator<iterator>;
    using const_resverse_iterator = std::reverse_iterator<const_iterator>;
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

    Char& at(size_t idx);

    long indexOf(Char c) const;
    
    long lastIndexOf(Char c) const;

    bool equals(const String& other, bool ignore_case = false) const;

    String toLower() const;

    String toLower2() const;

    String toUpper() const;

    String trimStart() const;

    String trimEnd() const;

    String trim() const;

    bool startsWith(Char c, bool ignore_case = false) const;

    bool startsWith(const String& str, bool ignore_case = false) const;

    bool endsWith(Char c, bool ignore_case = false) const;

    bool endsWith(const String& str, bool ignore_case = false) const;

    iterator begin() const;

    iterator end() const;

    const_iterator cbegin() const;

    const_iterator cend() const;

    resverse_iterator rbegin() const;

    resverse_iterator rend() const;

    const_resverse_iterator crbegin() const;
    
    const_resverse_iterator crend() const;
public:
    static String fromUtf8(const char* data);

    static String fromUtf16(const char16_t* data);

    static String fromLocal8Bit(const char* data);

public:
    String& operator =(const String& right);
    Char& operator[](size_t idx);
    bool operator ==(const String& right) const;
    bool operator != (const String& right) const;
    bool operator <(const String& right) const;
    bool operator >(const String& right) const;

    friend std::ostream& operator << (std::ostream& cout, const String& str);
    friend std::wostream& operator << (std::wostream& wcout, const String& str);
private:
    Char* data_ = nullptr;
    size_t len_ = 0;
};

VI_CORE_NS_END