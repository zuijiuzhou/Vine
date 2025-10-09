#include <vine/core/Exception.hpp>

#include <utility>

VI_CORE_NS_BEGIN

Exception::Exception() noexcept
{
}
Exception::Exception(Code code) noexcept
    : code_(code)
{
    switch (code)
    {
    case INDEX_OUT_OF_RANGE:
        msg_ = U"Index was out of range.";
        break;
    case ARGUMENT_NULL:
        msg_ = U"The parameter cannot be null.";
        break;
    default:
        break;
    }
}
Exception::Exception(Code code, String msg) noexcept
    : code_(code), msg_(std::move(msg))
{
}
Exception::Exception(int code) noexcept
    : code_(code)
{
}
Exception::Exception(int code, String msg) noexcept
    : code_(code), msg_(std::move(msg))
{
}

int Exception::code() const noexcept
{
    return code_;
}

const String &Exception::msg() const noexcept
{
    return msg_;
}

VI_CORE_NS_END