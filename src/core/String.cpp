#include <core/String.h>

#include <vector>

#include <core/Exception.h>

VINE_CORE_NS_BEGIN

namespace
{
    template <typename T>
    size_t getDataLength(const T *data)
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
}

String::String()
{
}

String::String(const Char *data)
{
    set(data);
}

String::String(const String &other)
{
    (*this) = other;
}

String::String(String &&other)
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

String String::substr(int start) const{
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
    throw Exception(Exception::IndexOutOfRange);
}

size_t String::size() const
{
    return len_;
}

size_t String::length() const
{
    return len_;
}

void String::clear()
{
    if (data_)
    {
        delete[] data_;
        data_ = nullptr;
        len_ = 0;
    }
}

bool String::empty() const
{
    return len_ == 0;
}

const Char *String::data() const
{
    return data_;
}

void String::set(const Char *data)
{
    set(data, getDataLength(data));
}

void String::set(const Char *data, size_t len)
{
    clear();
    if (data && len > 0)
    {
        auto temp = new Char[len + 1];
        memcpy(temp, data, len * sizeof(Char));
        temp[len] = 0;
        data_ = temp;
        len_ = len;
    }
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

String String::fromStdString(const std::string &data)
{
    return String();
}

String String::fromStdWString(const std::wstring &data)
{
    return String();
}

String &String::operator=(const String &right)
{
    set(right.data_, right.len_);
    return *this;
}

VINE_CORE_NS_END