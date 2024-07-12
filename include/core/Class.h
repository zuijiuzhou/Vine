#pragma once
#include "Std.h"
#include "core_global.h"

class type_info;

VI_CORE_NS_BEGIN

class VI_CORE_API Class {
  public:
    Class(const Class* parent, const type_info& ti);
    Class(const Class& cls)            = delete;
    Class(const Class&& cls)           = delete;
    Class& operator=(const Class& cls) = delete;

    const Class* parent() const noexcept;

    const Char* name() const noexcept;

    const Char* ns() const noexcept;

    const Char* fullName() const noexcept;

    bool isSubclassOf(const Class* cls) const;

    const type_info& ctype() const noexcept;

  public:
    bool operator==(const Class& right) const;
    bool operator!=(const Class& right) const;

  public:
    static Class* getClass(const type_info& ti);
    static Class* getClass(const Char* full_name);

  private:
    struct Data;
    Data* d;
};

using Type = const Class*;

VI_CORE_NS_END