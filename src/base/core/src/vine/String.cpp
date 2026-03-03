#include <vine/String.hpp>

#include <cstring>
#include <ctype.h>
#include <locale>
#include <vector>

#include <vine/Exception.hpp>

VI_CORE_NS_BEGIN

namespace
{

Char g_empty_buffer[] = { 0 };

template <typename T>
size_t get_data_len(const T* data)
{
    size_t len = 0;

    while (true) {
        if (*(data + len) == T(0))
            return len;
        len++;
    }

    return len;
}

inline bool is_white_char(Char c)
{
    return c == U' ' || c == U'\r' || c == U'\n' || c == U'\t' || c == U'\v' || c == U'\f';
}

inline bool is_same_char_ignore_case(Char l, Char r)
{
    if (l == r)
        return true;

    if (l >= U'A' && l <= U'Z')
        l += 32;

    if (r >= U'A' && r <= U'Z')
        r += 32;

    return l == r;
}

inline void delete_data(Char* data)
{
    if (data && data != g_empty_buffer) {
        delete[] data;
    }
}

} // namespace

String::String()
  : data_(g_empty_buffer)
{}

String::String(const Char* data)
{
    assign(data);
}

String::String(const Char* data, size_t count)
{
    assign(data, count);
}

String::String(const String& other) noexcept
{
    if (other.empty()) {
        clear();
    }
    else {
        assign(other.data_, other.len_);
    }
}

String::String(String&& other) noexcept
{
    delete_data(data_);

    data_     = other.data_;
    len_      = other.len_;
    capacity_ = other.capacity_;

    other.data_     = new Char[]{ 0 };
    other.len_      = 0;
    other.capacity_ = 0;
}

String::~String()
{
    delete_data(data_);
}

void String::resize(size_t new_size, Char ch)
{
    if (new_size == len_)
        return;

    if (new_size < len_) {
        len_        = new_size;
        data_[len_] = 0;
        return;
    }

    reserve(new_size);

    std::fill(data_ + len_, data_ + new_size, ch);

    len_        = new_size;
    data_[len_] = 0;
}

void String::reserve(size_t new_cap)
{
    if (new_cap <= capacity_)
        return;

    auto buf = new Char[new_cap + 1];

    if (len_ > 0)
        std::memcpy(buf, data_, len_ * sizeof(Char));

    buf[len_] = 0;

    delete_data(data_);

    data_     = buf;
    capacity_ = new_cap;
}

void String::swap(String& s) noexcept
{
    std::swap(data_, s.data_);
    std::swap(len_, s.len_);
    std::swap(capacity_, s.capacity_);
}

void String::shrinkToFit()
{
    if (capacity_ > len_ && len_ > 0) {
        auto buf = new Char[len_ + 1];
        memcpy(buf, data_, len_ * sizeof(Char));
        buf[len_] = 0;
        delete_data(data_);
        data_     = buf;
        capacity_ = len_;
    }
}

void String::clear() noexcept
{
    data_[0] = 0;
    len_     = 0;
}

String& String::assign(const Char* data, size_t len)
{
    if (!data || len == 0) {
        clear();
        return *this;
    }

    if (len == NPOS) {
        len = get_data_len(data);
    }

    // 自重合，且长度小于当前长度
    if (data_ == data) {
        if (len_ == len) {
            return *this;
        }

        if (len < len_) {
            data_[len] = 0;
            len_       = len;
            return *this;
        }
    }

    // 缓冲区小于长度，需要重新分配内存，自重合时没有问题
    if (capacity_ < len) {
        auto buf = new Char[len + 1];
        buf[len] = 0;

        memcpy(buf, data, len * sizeof(Char));

        delete_data(data_);
        data_     = buf;
        capacity_ = len;
    }
    else {
        // overlap
        if (data < data_ + len && data + len > data_) {
            memmove(data_, data, len * sizeof(Char));
        }
        else {
            memcpy(data_, data, len * sizeof(Char));
        }
        data_[len] = 0;
    }

    len_ = len;

    return *this;
}

String& String::assign(Char ch, size_t count)
{
    return *this;
}

String& String::assign(const String& str)
{
    return *this;
}

String& String::assign(const String& str, size_t pos, size_t count)
{
    return *this;
}

String& String::assign(String&& str)
{
    return *this;
}

String& String::assign(const_iterator begin, const_iterator end)
{
    return *this;
}

Char& String::at(size_t idx)
{
    if (idx >= len_)
        throw Exception(Exception::INDEX_OUT_OF_RANGE);

    return *(data_ + idx);
}

const Char& String::at(size_t idx) const
{
    if (idx >= len_)
        throw Exception(Exception::INDEX_OUT_OF_RANGE);

    return *(data_ + idx);
}

String String::substr(size_t pos, size_t count) const
{
    if (count == 0)
        return {};

    if (pos > len_)
        throw Exception(Exception::INDEX_OUT_OF_RANGE);

    count = std::min(count, len_ - pos);

    return String(data_ + pos, count);
}

size_t String::find(Char c, size_t pos) const
{
    for (size_t i = pos; i < len_; ++i) {
        if (data_[i] == c)
            return i;
    }
    return NPOS;
}

size_t String::rfind(Char c, size_t pos) const
{
    if (len_ == 0)
        return NPOS;

    if (pos == NPOS || pos > len_)
        pos = len_;

    for (size_t i = pos; i > 0; --i) {
        if (data_[i - 1] == c)
            return i - 1;
    }

    return NPOS;
}

String String::toLower(bool ascii_only) const
{
    if (len_ == 0)
        return *this;

    auto s = *this;

    if (ascii_only) {
        for (size_t i = 0; i < s.len_; i++) {
            auto& c = s.at(i);

            if (c >= U'A' && c <= U'Z')
                c += 32;
        }
    }
    else {
        for (size_t i = 0; i < s.len_; i++) {
            auto& c = s.at(i);

            if (c >= U'A' && c <= U'Z')
                c += 32;
        }
        // auto& f = std::use_facet<std::ctype<char32_t>>(std::locale(""));
        // f.tolower(s.data_, s.data_ + s.len_);
    }
    return s;
}

String String::toUpper(bool ascii_only) const
{
    if (len_ == 0)
        return *this;

    auto s = *this;

    if (ascii_only) {
        for (size_t i = 0; i < s.len_; i++) {
            auto& c = s.at(i);

            if (c >= U'a' && c <= U'z')
                c -= 32;
        }
    }
    else {
        for (size_t i = 0; i < s.len_; i++) {
            auto& c = s.at(i);

            if (c >= U'a' && c <= U'z')
                c -= 32;
        }
        // auto& f = std::use_facet<std::ctype<char32_t>>(std::locale(""));
        // f.toupper(s.data_, s.data_ + s.len_);
    }
    return s;
}

String& String::trimStart()
{
    if (len_ == 0)
        return *this;

    size_t i = 0;
    while (i < len_ && is_white_char(data_[i])) ++i;

    if (i == 0)
        return *this;

    if (i == len_) {
        len_     = 0;
        data_[0] = 0;
        return *this;
    }

    len_ -= i;
    std::memmove(data_, data_ + i, len_ * sizeof(Char));
    data_[len_] = 0;

    return *this;
}

String String::trimmedStart() const
{
    if (empty())
        return {};

    size_t i = 0;

    while (i < len_ && is_white_char(data_[i])) ++i;

    if (i == 0)
        return *this; // 没有前导空白，直接返回副本

    if (i == len_)
        return {}; // 全是空白

    return String(data_ + i, len_ - i);
}

String& String::trimEnd()
{
    if (empty())
        return *this;

    size_t i = len_;

    while (i > 0 && is_white_char(data_[--i])) {
    }

    if (i != len_) {
        len_     = i;
        data_[i] = 0;
    }

    return *this;
}

String String::trimmedEnd() const
{
    if (empty())
        return {};

    size_t i = len_;

    while (i > 0 && is_white_char(data_[--i])) {
    }

    if (i == len_)
        return *this;

    if (i == 0)
        return {};

    return String(data_, i);
}

String& String::trim()
{
    if (empty())
        return *this;

    size_t start = 0;
    size_t end   = len_;

    while (start < end && is_white_char(data_[start])) ++start;

    while (end > start && is_white_char(data_[end - 1])) --end;

    if (start == 0 && end == len_)
        return *this;

    if (start == end) {
        len_     = 0;
        data_[0] = 0;
        return *this;
    }

    size_t new_len = end - start;

    if (start > 0)
        std::memmove(data_, data_ + start, new_len * sizeof(Char));

    len_        = new_len;
    data_[len_] = 0;

    return *this;
}

String String::trimmed() const
{
    if (empty())
        return {};

    size_t start = 0;
    size_t end   = len_;

    while (start < end && is_white_char(data_[start])) ++start;
    while (end > start && is_white_char(data_[end - 1])) --end;

    if (start == 0 && end == len_)
        return *this;

    if (start == end)
        return {};

    return String(data_ + start, end - start);
}

bool String::equals(const String& other, bool ignore_case) const
{
    if (len_ == 0 && other.len_ == 0)
        return true;

    if (len_ != other.len_)
        return false;

    if (ignore_case) {
        for (size_t i = 0; i < len_; ++i) {
            if (!is_same_char_ignore_case(data_[i], other.data_[i]))
                return false;
        }
    }
    else {
        return memcmp(data_, other.data_, len_ * sizeof(Char)) == 0;
    }

    return true;
}

bool String::startsWith(Char c, bool ignore_case) const
{
    if (len_ == 0)
        return false;

    if (ignore_case) {
        return is_same_char_ignore_case(*data_, c);
    }
    else {
        return *data_ == c;
    }
}

bool String::startsWith(const String& str, bool ignore_case) const
{
    if (str.len_ == 0)
        return true;

    if (len_ < str.len_)
        return false;

    if (ignore_case)
        return memcmp(data_, str.data_, str.len_ * sizeof(Char)) == 0;

    for (size_t i = 0; i < str.len_; i++) {
        if (is_same_char_ignore_case(data_[i], str.data_[i]))
            continue;

        return false;
    }

    return true;
}

bool String::endsWith(Char c, bool ignore_case) const
{
    if (len_ == 0)
        return false;

    if (ignore_case) {
        return is_same_char_ignore_case(c, data_[len_ - 1]);
    }
    else {
        return c == data_[len_ - 1];
    }
}

bool String::endsWith(const String& str, bool ignore_case) const
{
    if (str.len_ == 0)
        return true;

    if (len_ < str.len_)
        return false;

    if (ignore_case)
        return memcmp(data_ + len_ - str.len_, str.data_, str.len_ * sizeof(Char)) == 0;

    for (size_t i = 0; i < str.len_; i++) {
        if (is_same_char_ignore_case(data_[len_ - i - 1], str.data_[str.len_ - i - 1]))
            continue;

        return false;
    }
    return true;
}

String String::fromUtf8(const char* data)
{
    if (!data)
        return {};

    String s;

    unsigned char         c;
    size_t                i, n;
    char32_t              c32;
    std::vector<char32_t> u32;

    while (*data) {
        c = *data++;
        if (c < 0x80) {
            c32 = c;
            n   = 1;
        }
        else if (c < 0xC0 || c > 0xFD) {
            break;
        }
        else if (c < 0xE0) {
            c32 = c & 0x1F;
            n   = 2;
        }
        else if (c < 0xF0) {
            c32 = c & 0x0F;
            n   = 3;
        }
        else if (c < 0xF8) {
            c32 = c & 7;
            n   = 4;
        }
        else if (c < 0xFC) {
            c32 = c & 3;
            n   = 5;
        }
        else {
            c32 = c & 1;
            n   = 6;
        }
        for (i = 1; i < n; i++) {
            c = *data++;
            if (c < 0x80 || c > 0xBF)
                break;
            c32 = (c32 << 6) + (c & 0x3F);
        }
        u32.push_back(c32);
    }
    s.assign(u32.data(), u32.size());
    return s;
}

String String::fromUtf16(const char16_t* data)
{
    return String();
}

String String::fromLocal8Bit(const char* data)
{
    return String();
}

String& String::operator=(const String& right)
{
    assign(right.data_, right.len_);
    return *this;
}

bool String::operator==(const String& right) const
{
    return equals(right);
}

bool String::operator!=(const String& right) const
{
    return !(*this == right);
}

bool String::operator<(const String& right) const
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

bool String::operator>(const String& right) const
{
    return false;
}

String String::operator+(const String& right) const
{
    if (right.len_ == 0)
        return *this;

    if (len_ == 0)
        return right;

    String result;

    return result;
}

String& String::operator+=(const String& right)
{
    return *this;
}

VI_CORE_NS_END