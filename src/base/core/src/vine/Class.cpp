#include <vine/Class.hpp>

#include <mutex>
#include <set>
#include <typeinfo>
#include <algorithm>
#include <stdexcept>

#ifdef __GCC__
#include <cxxabi.h>
#endif

#include <vine/Exception.hpp>

VI_CORE_NS_BEGIN

namespace {

#ifdef __MSVC__
bool parse_type_info_vc(const std::type_info& ti, String& name, String& ns, String& full_name) {
    auto n    = ti.name();
    full_name = String::fromUtf8(n);
    full_name = full_name.substr(6);
    auto idx  = full_name.lastIndexOf(U':');
    name      = full_name.substr(idx + 1);
    ns        = full_name.substr(0, idx - 1);
    return true;
}
#endif

#ifdef __GCC__
bool parse_type_info_gcc(const std::type_info& ti, String& name, String& ns, String& full_name) {
    int status;
    char* demangled = abi::__cxa_demangle(ti.name(), nullptr, nullptr, &status);

    if(status != 0)
    return false;

    full_name = String::fromUtf8(demangled);
    delete demangled;

    size_t pos = full_name.lastIndexOf(U':');

    if(pos == -1) {
        name = full_name;
    } else {
        ns = full_name.substr(0, pos - 1);
        name = full_name.substr(pos + 1);
    }

    return true;
}
#endif

} // namespace

struct Class::Data {
    Data(const std::type_info& i)
      : ti(i) {}
    const Class*            parent = nullptr;
    const std::type_info&   ti;
    String                  name;
    String                  ns;
    String                  full_name;
    static std::set<Class*> classes;
};

std::set<Class*> Class::Data::classes = {};

Class::Class(const Class* parent, const std::type_info& ti)
  : d(new Data(ti)) {
    if (getClass(ti)) 
        throw Exception(Exception::ITEM_ALREADY_EXISTS);
    d->parent = parent;

    auto is_ok = false;

#if defined(__MSVC__)
    is_ok = parse_type_info_vc(ti, d->name, d->ns, d->full_name);
#elif defined(__GCC__)
    is_ok = parse_type_info_gcc(ti, d->name, d->ns, d->full_name);
#else
    #error "Unsupported compiler"
#endif

    if(!is_ok){
        throw std::runtime_error("Runtime error.");
    }

    Data::classes.insert(this);
}

Class::~Class() {
    Data::classes.erase(this);
}

const Class* Class::parent() const noexcept {
    return d->parent;
}

const Char* Class::name() const noexcept {
    return d->name.data();
}

const Char* Class::ns() const noexcept {
    return d->ns.data();
}

const Char* Class::fullName() const noexcept {
    return d->full_name.data();
}

const std::type_info& Class::ctype() const noexcept {
    return d->ti;
}

bool Class::isSubclassOf(const Class* cls) const {
    if (cls == nullptr) return false;

    auto type = this;
    do {
        if (type == cls) return true;
        type = type->parent();
    } while (type);
    return false;
}

Class* Class::getClass(const std::type_info& ti) {
    auto iter = std::find_if(Data::classes.begin(), Data::classes.end(), [&ti](Class* c) { return c->ctype() == ti; });
    if (iter == Data::classes.end()) return nullptr;
    return *iter;
}

Class* Class::getClass(const Char* full_name) {
    auto iter = std::find_if(Data::classes.begin(), Data::classes.end(), [&full_name](Class* c) {
        return c->fullName() == full_name;
    });
    if (iter == Data::classes.end()) return nullptr;
    return *iter;
}

bool Class::operator==(const Class& right) const {
    return d->ti == right.d->ti;
}

bool Class::operator!=(const Class& right) const {
    return !(*this == right);
}

VI_CORE_NS_END
