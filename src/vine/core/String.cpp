#include <vine/core/String.h>

#include <vector>
#include <locale>
#include <ctype.h>
#include <cstring>

#include <vine/core/Exception.h>

VI_CORE_NS_BEGIN

namespace
{
    template <typename T>
    size_t get_data_len(const T *data)
    {
        size_t len = 0;
        while (true)
        {
            if (*(data + len) == 0)
                return len;
            len++;
        }
        return len;
    }

    inline bool is_white_char(Char c)
    {
        return c == U' ' || c == U'\r' || c == U'\n' || c == U'\t';
    }
}

const String String::E = U"";

String::String()
{
    clear();
}

String::String(const Char *data)
{
    set(data);
}

String::String(const String &other) noexcept
{
    (*this) = other;
}

String::String(String &&other) noexcept
{
    if (data_)
        delete[] data_;
    data_ = other.data_;
    other.data_ = nullptr;
}

String::~String()
{
    clear();
}

String String::substr(int start) const
{
    return substr(start, len_ - start);
}

String String::substr(int start, int count) const
{
    String s;
    if (len_ == 0)
        return s;
    if (start >= 0 && count >= 0 && start <= len_ && start + count <= len_)
    {
        if (start != len_)
            s.set(data_ + start, count);
        return s;
    }
    throw Exception(Exception::INDEX_OUT_OF_RANGE);
}

size_t String::size() const noexcept
{
    return len_;
}

size_t String::length() const noexcept
{
    return len_;
}

void String::clear() noexcept
{
    if (data_)
    {
        delete[] data_;
        data_ = nullptr;
        len_ = 0;
    }
    data_ = new Char[1]{0};
}

bool String::empty() const noexcept
{
    return len_ == 0;
}

const Char *String::data() const noexcept
{
    return data_;
}

void String::set(const Char *data)
{
    set(data, get_data_len(data));
}

void String::set(const Char *data, size_t len)
{
    clear();
    if (data && len > 0)
    {
        auto temp = new Char[len + 1];
        memcpy(temp, data, len * sizeof(Char));
        temp[len] = 0;
        if(data_){
            delete[] data_;
        }
        data_ = temp;
        len_ = len;
    }
}

Char &String::at(size_t idx)
{
    if (idx >= len_)
        throw Exception(Exception::INDEX_OUT_OF_RANGE);
    return *(data_ + idx);
}

long String::indexOf(Char c) const
{
    if (empty())
        return -1;

    size_t idx = 0;
    while (idx < len_)
    {
        if (*(data_ + idx) == c)
            return idx;
        idx++;
    }
    return -1;
}

long String::lastIndexOf(Char c) const
{
    if (empty())
        return -1;

    auto idx = len_;
    while (idx > 0)
    {
        if (*(data_ + --idx) == c)
            return idx;
    }
    return -1;
}

bool String::equals(const String &other, bool ignore_case) const
{
    if (empty() && other.empty())
        return true;
    if (len_ != other.len_)
        return false;
    if (ignore_case)
    {
        for (size_t i = 0; i < len_; i++)
        {
            auto &c1 = *(data_ + i);
            auto &c2 = *(other.data_ + i);
            if (!(c1 == c2 || c1 - c2 != 32 || c1 - c2 != -32))
                return false;
        }
    }
    else
    {
        return memcmp(data_, other.data_, len_ * sizeof(Char)) == 0;
    }
    return true;
}

String String::toLower(bool is_asc_only) const
{
    if (len_ == 0)
        return *this;

    auto s = *this;
    if (is_asc_only) {
        for (size_t i = 0; i < s.len_; i++)
        {
            auto& c = s.at(i);
            if (c >= 65 && c <= 90)
                c += 32;
        }
    }
    else {
        auto& f = std::use_facet<std::ctype<char32_t>>(std::locale(""));
        f.tolower(s.data_, s.data_ + s.len_);
    }
    return s;
}

String String::toUpper(bool is_asc_only) const
{
    if (len_ == 0)
        return *this;

    auto s = *this;
    if (is_asc_only) {
        for (size_t i = 0; i < s.len_; i++)
        {
            auto& c = s.at(i);
            if (c >= 97 && c <= 122)
                c -= 32;
        }
    }
    else {
        auto& f = std::use_facet<std::ctype<char32_t>>(std::locale(""));
        f.toupper(s.data_, s.data_ + s.len_);
    }
    return s;
}

String String::trimStart() const
{
    String s;
    if (empty())
        return s;
    auto len = len_;
    auto data = data_;
    while (len > 0)
    {
        if (is_white_char(*data))
        {
            len--;
            data++;
            continue;
        }
        break;
    }
    if (len == 0)
        return s;
    s.set(data, len);
    return s;
}

String String::trimEnd() const
{
    String s;
    if (empty())
        return s;
    auto len = len_;
    auto data = (data_ + len_ - 1);
    while (len > 0)
    {
        if (is_white_char(*data))
        {
            len--;
            data--;
            continue;
        }
        break;
    }
    if (len == 0)
        return s;
    s.set(data_, len);
    return s;
}

String String::trim() const
{
    String s;
    if (empty())
        return s;
    auto len = len_;
    auto data_start = data_;
    while (len > 0)
    {
        if (is_white_char(*data_start))
        {
            len--;
            data_start++;
            continue;
        }
        break;
    }
    if (len == 0)
        return s;
    auto data_end = (data_ + len_ - 1);
    while (len > 0)
    {
        if (is_white_char(*data_end))
        {
            len--;
            data_end--;
            continue;
        }
        break;
    }
    if (len == 0)
        return s;
    s.set(data_start, len);
    return s;
}

bool String::startsWith(Char c, bool ignore_case) const
{
    if (empty())
        return false;
    auto n = c > *data_ ? c - *data_ : *data_ - c;
    if (ignore_case)
    {
        return n == 0 || n == 32;
    }
    else
    {
        return n == 0;
    }
}

bool String::startsWith(const String &str, bool ignore_case) const
{
    if (str.empty())
        return true;
    if (length() < str.length())
        return false;

    for (size_t i = 0; i < str.length(); i++)
    {
        auto c1 = *(data_ + i);
        auto c2 = *(str.data_ + i);

        auto n = c1 > c2 ? c1 - c2 : c2 - c1;
        if (n == 0 || (ignore_case && n == 32))
        {
            continue;
        }
        return false;
    }

    return true;
}

bool String::endsWith(Char c, bool ignore_case) const
{
    if (empty())
        return false;
    auto &c_last = *(data_ + len_ - 1);
    auto n = c > c_last ? c - c_last : c_last - c;
    if (ignore_case)
    {
        return n == 0 || n == 32;
    }
    else
    {
        return n == 0;
    }
}

bool String::endsWith(const String &str, bool ignore_case) const
{
    if (str.empty())
        return true;
    if (length() < str.length())
        return false;

    auto data1 = data_ + len_ - 1;
    auto data2 = str.data_ + str.len_ - 1;
    for (size_t i = 0; i < str.length(); i++)
    {
        auto c1 = *(data1--);
        auto c2 = *(data2--);

        auto n = c1 > c2 ? c1 - c2 : c2 - c1;
        if (n == 0 || (ignore_case && n == 32))
        {
            continue;
        }
        return false;
    }

    return true;
}

String::iterator String::begin() const
{
    return data_;
}

String::iterator String::end() const
{
    return data_ + len_;
}

String::const_iterator String::cbegin() const
{
    return data_;
}

String::const_iterator String::cend() const
{
    return data_ + len_;
}

String String::fromUtf8(const char *data)
{
    String s;

    if (!data)
        return s;

    unsigned char c;
    size_t i, n;
    char32_t c32;
    std::vector<char32_t> u32;

    while (*data)
    {
        c = *data++;
        if (c < 0x80)
        {
            c32 = c;
            n = 1;
        }
        else if (c < 0xC0 || c > 0xFD)
        {
            break;
        }
        else if (c < 0xE0)
        {
            c32 = c & 0x1F;
            n = 2;
        }
        else if (c < 0xF0)
        {
            c32 = c & 0x0F;
            n = 3;
        }
        else if (c < 0xF8)
        {
            c32 = c & 7;
            n = 4;
        }
        else if (c < 0xFC)
        {
            c32 = c & 3;
            n = 5;
        }
        else
        {
            c32 = c & 1;
            n = 6;
        }
        for (i = 1; i < n; i++)
        {
            c = *data++;
            if (c < 0x80 || c > 0xBF)
                break;
            c32 = (c32 << 6) + (c & 0x3F);
        }
        u32.push_back(c32);
    }
    s.set(u32.data(), u32.size());
    return s;
}

String String::fromUtf16(const char16_t *data)
{
    return String();
}

String String::fromLocal8Bit(const char *data)
{
    return String();
}

String &String::operator=(const String &right)
{
    set(right.data_, right.len_);
    return *this;
}

Char &String::operator[](size_t idx)
{
    return at(idx);
}

bool String::operator==(const String &right) const
{
    return equals(right);
}

bool String::operator!=(const String &right) const
{
    return !(*this == right);
}

bool String::operator<(const String &right) const
{
    if (data_ && right.data_) {
        return memcmp(data_, right.data_, std::min(len_, right.len_));
    }
    else if (data_) {
        
    }
    else if (right.data_) {

    }
    else {
        return false;
    }
    return false;
}

bool String::operator>(const String &right) const
{
    return false;
}

VI_CORE_NS_END