#pragma once
#include "Std.h"
#include "core_global.h"

namespace std{
class type_info;
}

VI_CORE_NS_BEGIN

class VI_CORE_API Class final{
  public:
    Class(const Class* parent, const std::type_info& ti);
    Class(const Class& cls)            = delete;
    Class(const Class&& cls)           = delete;
    Class& operator=(const Class& cls) = delete;
    ~Class();

    const Class* parent() const noexcept;

    const Char* name() const noexcept;

    const Char* ns() const noexcept;

    const Char* fullName() const noexcept;

    bool isSubclassOf(const Class* cls) const;

    const std::type_info& ctype() const noexcept;

  public:
    bool operator==(const Class& right) const;
    bool operator!=(const Class& right) const;

  public:
    static Class* getClass(const std::type_info& ti);
    static Class* getClass(const Char* full_name);

  private:
    struct Data;
    Data* d;
};

using Type = const Class*;
using TypeInfo = const Class*;

VI_CORE_NS_END