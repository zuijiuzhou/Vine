#include <vine/Class.hpp>

#include <algorithm>
#include <mutex>
#include <set>
#include <stdexcept>
#include <typeinfo>

#if defined(__GNUC__)
#    include <cxxabi.h>
#endif

#include <vine/Exception.hpp>

V_CORE_NS_BEGIN

namespace
{

std::set<Class*> s_classes;
std::mutex       s_classes_mutex;

#if defined(_MSC_VER)
bool parse_type_info_msvc(const std::type_info& c_type, String& name, String& ns, String& full_name)
{
    auto n    = c_type.name();
    full_name = String::fromLocal8Bit(n);
    full_name = full_name.substr(6);
    auto idx  = full_name.rfind(u8"::");
    name      = full_name.substr(idx + 2);
    ns        = full_name.substr(0, idx);
    return true;
}
#endif

#if defined(__GNUC__)
// clang defines __GNUC__ as well, so this is used for both GCC and clang
bool parse_type_info_gnuc(const std::type_info& c_type, String& name, String& ns, String& full_name)
{
    int   status;
    char* demangled = abi::__cxa_demangle(c_type.name(), nullptr, nullptr, &status);

    if (status != 0)
        return false;

    full_name = reinterpret_cast<char8_t*>(demangled);
    free(demangled);

    size_t pos = full_name.rfind(u8"::");

    if (pos == -1) {
        name = full_name;
    }
    else {
        ns   = full_name.substr(0, pos);
        name = full_name.substr(pos + 2);
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

#if defined(_MSC_VER)
    is_ok = parse_type_info_msvc(c_type, name_, ns_, full_name_);
#elif defined(__GNUC__)
    is_ok = parse_type_info_gnuc(c_type, name_, ns_, full_name_);
#else
#    error "Unsupported compiler"
#endif

    if (!is_ok) {
        throw std::runtime_error("Runtime error.");
    }

    {
        std::lock_guard<std::mutex> lock(s_classes_mutex);
        s_classes.insert(this);
    }
}

Class::~Class()
{
    {
        std::lock_guard<std::mutex> lock(s_classes_mutex);
        s_classes.erase(this);
    }
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

bool Class::operator==(const Class& right) const noexcept
{
    return c_type_ == right.c_type_;
}

bool Class::operator!=(const Class& right) const noexcept
{
    return !(*this == right);
}

Class* Class::getClass(const std::type_info& c_type)
{
    std::lock_guard<std::mutex> lock(s_classes_mutex);
    auto                        it = std::find_if(s_classes.begin(), s_classes.end(), [&c_type](Class* c) { return c->c_type_ == c_type; });
    if (it == s_classes.end())
        return nullptr;
    return *it;
}

Class* Class::getClass(const String& full_name)
{
    std::lock_guard<std::mutex> lock(s_classes_mutex);
    auto                        it = std::find_if(s_classes.begin(), s_classes.end(), [&full_name](Class* c) { return c->full_name_ == full_name; });
    if (it == s_classes.end())
        return nullptr;
    return *it;
}

V_CORE_NS_END
