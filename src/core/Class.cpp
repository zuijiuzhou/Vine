#include <core/Class.h>

#include <mutex>
#include <set>
#include <typeinfo>

#include <core/Exception.h>

VI_CORE_NS_BEGIN

namespace {
bool parse_type_info_vc(const type_info& ti, String& name, String& ns, String& full_name) {
    auto n    = ti.name();
    auto rn   = ti.raw_name();
    full_name = String::fromUtf8(n);
    full_name = full_name.substr(6);
    auto idx  = full_name.lastIndexOf(U':');
    name      = full_name.substr(idx + 1);
    ns        = full_name.substr(0, idx - 1);
    return true;
}
} // namespace

struct Class::Data {
    Data(const type_info& i)
      : ti(i) {}
    const Class*            parent = nullptr;
    const type_info&        ti;
    String                  name;
    String                  ns;
    String                  full_name;
    static std::set<Class*> classes;
};

std::set<Class*> Class::Data::classes = {};

Class::Class(const Class* parent, const type_info& ti)
  : d(new Data(ti)) {
    if (getClass(ti)) throw Exception(Exception::ITEM_ALREADY_EXISTS);
    d->parent = parent;
#if defined(_MSC_VER)
    parse_type_info_vc(ti, d->name, d->ns, d->full_name);
#else
#    error "NOT IMPLEMENTED!"
#endif
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

const type_info& Class::ctype() const noexcept {
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

Class* Class::getClass(const type_info& ti) {
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
