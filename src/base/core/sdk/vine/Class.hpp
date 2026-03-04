#pragma once
#include "core_global.hpp"

#include "String.hpp"

#if defined(_MSC_VER)
class type_info;

namespace std
{

using type_info = ::type_info;

}
#else
namespace std
{

class type_info;

}
#endif

VI_CORE_NS_BEGIN

class VI_CORE_API Class final {
  public:
    Class(const std::type_info& ti, const Class* parent);
    Class(const Class& cls)            = delete;
    Class(const Class&& cls)           = delete;
    Class& operator=(const Class& cls) = delete;
    ~Class();

    const Class* parent() const noexcept
    {
        return parent_;
    }

    const String& name() const noexcept
    {
        return name_;
    }

    const String& ns() const noexcept
    {
        return ns_;
    }

    const String& fullName() const noexcept
    {
        return full_name_;
    }

    const std::type_info& ctype() const noexcept
    {
        return c_type_;
    }

    bool isSubclassOf(const Class* cls) const noexcept;

  public:
    bool operator==(const Class& right) const noexcept;
    bool operator!=(const Class& right) const noexcept;

  public:
    static Class* getClass(const std::type_info& ti);
    static Class* getClass(const String& full_name);

  private:
    const std::type_info& c_type_;
    String                name_;
    String                ns_;
    String                full_name_;
    const Class*          parent_ = nullptr;
};

using Type     = const Class*;
using TypeInfo = const Class*;

VI_CORE_NS_END
