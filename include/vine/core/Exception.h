#pragma once

#include "core_global.h"

#include "String.h"

VI_CORE_NS_BEGIN

class VI_CORE_API Exception {
  public:
    enum Code
    {
        SYSTEM_ERROR = 0x1,
        INDEX_OUT_OF_RANGE,
        ITEM_ALREADY_EXISTS,
        ARGUMENT_NULL,
        INVALID_ARGUMENTS,
        INVALID_OPERATION,
        NOT_SUPPORT,
        NOT_IMPLEMENTED,

        USER_EXCEPTION = 0x800001
    };

  public:
    Exception() noexcept;
    explicit Exception(Code code) noexcept;
    explicit Exception(Code code, String msg) noexcept;
    explicit Exception(int code) noexcept;
    explicit Exception(int code, String msg) noexcept;

  public:
    int           code() const noexcept;
    const String& msg() const noexcept;

  private:
    int    code_;
    String msg_;
};

VI_CORE_NS_END

#define VI_CHECK_NULL_THROW(var)                                                                                       \
    if (!var) throw vine::Exception(vine::Exception::ARGUMENT_NULL, U## #var);
#define VI_CHECK_NULL_RETURN(var)                                                                                      \
    if (!var) return {};