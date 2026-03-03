#pragma once
#include "core_global.hpp"

#include <cstddef>

#include "Char.hpp"

VI_CORE_NS_BEGIN

class VI_CORE_API String final {

  public:
    using size_type      = size_t;
    using iterator       = Char*;
    using const_iterator = const Char*;

  public:
    String();
    String(const Char* data);
    String(const Char* data, size_t count);
    String(const String& other) noexcept;
    String(String&& other) noexcept;
    virtual ~String();

  public:
    static constexpr size_t NPOS = size_t(-1);

  public:
    size_t size() const noexcept
    {
        return len_;
    }

    size_t length() const noexcept
    {
        return len_;
    }

    size_t capacity() const noexcept
    {
        return capacity_;
    }

    bool empty() const noexcept
    {
        return len_ == 0;
    }

    Char* data() noexcept
    {
        return data_;
    }

    const Char* data() const noexcept
    {
        return data_;
    }

    void clear() noexcept;

    void resize(size_t new_size, Char ch = {});

    void reserve(size_t new_cap);

    void shrinkToFit();

    void swap(String& s) noexcept;

    String& assign(Char ch, size_t count);

    String& assign(const String& str);

    String& assign(const String& str, size_t pos, size_t count);

    String& assign(String&& str);

    String& assign(const_iterator begin, const_iterator end);

    String& assign(const Char* data, size_t count);

    Char& at(size_t idx);

    const Char& at(size_t idx) const;

    String substr(size_t start, size_t count = NPOS) const;

    size_t find(Char c, size_t pos = 0) const;

    size_t rfind(Char c, size_t pos = NPOS) const;

    String toLower(bool ascii_only = true) const;

    String toUpper(bool ascii_only = true) const;

    String& trimStart();

    String trimmedStart() const;

    String trimmedEnd() const;

    String& trimEnd();

    String& trim();

    String trimmed() const;

    bool equals(const String& other, bool ignore_case = false) const;

    bool startsWith(Char c, bool ignore_case = false) const;

    bool startsWith(const String& str, bool ignore_case = false) const;

    bool endsWith(Char c, bool ignore_case = false) const;

    bool endsWith(const String& str, bool ignore_case = false) const;

    iterator begin() const
    {
        return data_;
    }

    iterator end() const
    {
        return data_ + len_;
    }

    const_iterator cbegin() const
    {
        return data_;
    }

    const_iterator cend() const
    {
        return data_ + len_;
    }

  public:
    static String fromUtf8(const char* data);

    static String fromUtf16(const char16_t* data);

    static String fromLocal8Bit(const char* data);

  public:
    String& operator=(const String& right);

    Char& operator[](size_t idx)
    {
        return *(data_ + idx);
    }

    const Char& operator[](size_t idx) const
    {
        return *(data_ + idx);
    }

    bool operator==(const String& right) const;
    bool operator!=(const String& right) const;
    bool operator<(const String& right) const;
    bool operator>(const String& right) const;

    String  operator+(const String& right) const;
    String& operator+=(const String& right);

  private:
    Char*  data_{};
    size_t len_{};
    size_t capacity_{};
};

VI_CORE_NS_END