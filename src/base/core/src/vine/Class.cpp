#include <vine/Class.hpp>

#include <algorithm>
#include <mutex>
#include <set>
#include <stdexcept>
#include <typeinfo>

#ifdef __GCC__
#    include <cxxabi.h>
#endif

#include <vine/Exception.hpp>

VI_CORE_NS_BEGIN

namespace
{

// todo:: thread safety
std::set<Class*> s_classes;

#ifdef __MSVC__
bool parse_type_info_vc(const std::type_info& c_type, String& name, String& ns, String& full_name)
{
    auto n    = c_type.name();
    full_name = String::fromLocal8Bit(n);
    full_name = full_name.substr(6);
    auto idx  = full_name.rfind(u8':');
    name      = full_name.substr(idx + 1);
    ns        = full_name.substr(0, idx - 1);
    return true;
}
#endif

#ifdef __GCC__
bool parse_type_info_gcc(const std::type_info& c_type, String& name, String& ns, String& full_name)
{
    int   status;
    char* demangled = abi::__cxa_demangle(c_type.name(), nullptr, nullptr, &status);

    if (status != 0)
        return false;

    full_name = reinterpret_cast<char8_t*>(demangled);
    free(demangled);

    size_t pos = full_name.rfind(u8':');

    if (pos == -1) {
        name = full_name;
    }
    else {
        ns   = full_name.substr(0, pos - 1);
        name = full_name.substr(pos + 1);
    }

    return true;
}
#endif

} // namespace

Class::Class(const std::type_info& c_type, const Class* parent)
  : c_type_(c_type)
{
    if (getClass(c_type))
        throw Exception(Exception::ITEM_ALREADY_EXISTS);
    this->parent_ = parent;

    auto is_ok = false;

#if defined(__MSVC__)
    is_ok = parse_type_info_vc(c_type, name_, ns_, full_name_);
#elif defined(__GCC__)
    is_ok = parse_type_info_gcc(c_type, name_, ns_, full_name_);
#else
#    error "Unsupported compiler"
#endif

    if (!is_ok) {
        throw std::runtime_error("Runtime error.");
    }

    s_classes.insert(this);
}

Class::~Class()
{
    s_classes.erase(this);
}

bool Class::isSubclassOf(const Class* cls) const noexcept
{
    if (cls == nullptr)
        return false;

    auto self = this;
    do {
        if (self == cls)
            return true;
        self = self->parent_;
    }
    while (self);
    return false;
}

Class* Class::getClass(const std::type_info& c_type)
{
    auto it = std::find_if(s_classes.begin(), s_classes.end(), [&c_type](Class* c) { return c->c_type_ == c_type; });
    if (it == s_classes.end())
        return nullptr;
    return *it;
}

Class* Class::getClass(const String& full_name)
{
    auto it = std::find_if(s_classes.begin(), s_classes.end(), [&full_name](Class* c) { return c->full_name_ == full_name; });
    if (it == s_classes.end())
        return nullptr;
    return *it;
}

bool Class::operator==(const Class& right) const noexcept
{
    return c_type_ == right.c_type_;
}

bool Class::operator!=(const Class& right) const noexcept
{
    return !(*this == right);
}

VI_CORE_NS_END
