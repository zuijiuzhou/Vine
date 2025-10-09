#pragma once
#include "core_global.hpp"

#include <cstddef>

#include "Char.hpp"

VI_CORE_NS_BEGIN

class VI_CORE_API String final {

  public:
    using iterator                = Char*;
    using const_iterator          = const Char*;

  public:
    String();
    String(const Char* data);
    String(const String& other) noexcept;
    String(String&& other) noexcept;
    virtual ~String();
    
  public:
    String substr(int start) const;

    String substr(int start, int count) const;

    size_t size() const noexcept;

    size_t length() const noexcept;

    void clear() noexcept;

    bool empty() const noexcept;

    const Char* data() const noexcept;

    void set(const Char* data);

    void set(const Char* data, size_t len);

    Char& at(size_t idx);

    long indexOf(Char c) const;

    long lastIndexOf(Char c) const;

    bool equals(const String& other, bool ignore_case = false) const;

    String toLower(bool is_asc_only = true) const;

    String toUpper(bool is_asc_only = true) const;

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

  public:
    static String fromUtf8(const char* data);

    static String fromUtf16(const char16_t* data);

    static String fromLocal8Bit(const char* data);

  public:
    String& operator=(const String& right);
    Char&   operator[](size_t idx);
    bool    operator==(const String& right) const;
    bool    operator!=(const String& right) const;
    bool    operator<(const String& right) const;
    bool    operator>(const String& right) const;

  public:
    static const String E;

  private:
    Char*  data_ = nullptr;
    size_t len_  = 0;
};

VI_CORE_NS_END