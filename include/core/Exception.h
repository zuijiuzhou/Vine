#pragma once

#include "core_global.h"

#include "String.h"

VINE_CORE_NS_BEGIN

class VINE_CORE_API Exception
{
public:
    enum Code {
        IndexOutOfRange = 0x10001,
        ArgumentNull,
        ItemAlreadyExists,
    };

public:
    Exception() noexcept;
    explicit Exception(Code code) noexcept;
    explicit Exception(Code code, String msg) noexcept;
    explicit Exception(int code) noexcept;
    explicit Exception(int code, String msg) noexcept;

public:
    int code() const noexcept;
    const String& msg() const noexcept;

private:
    int code_;
    String msg_;
};

VINE_CORE_NS_END