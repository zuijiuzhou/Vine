#include <core/Class.h>

#include <set>
#include <typeinfo>

#include <core/Exception.h>

VINE_CORE_NS_BEGIN

namespace
{
    bool parseNameAndNS(const type_info &ti, String &name, String &ns, String &full_name)
    {
        auto n = ti.name();
        auto rn = ti.raw_name();
        full_name = String::fromUtf8(n);
        full_name = full_name.substr(6);
        auto idx = full_name.lastIndexOf(U':');
        name = full_name.substr(idx + 1);
        ns = full_name.substr(0, idx - 1);
        return true;
    }
}

struct Class::Data
{
    Data(const type_info &i) : ti(i)
    {

    }
    const Class *parent = nullptr;
    const type_info &ti;
    String name;
    String ns;
    String full_name;
    static std::set<Class *> classes;
};

std::set<Class *> Class::Data::classes = {};

Class::Class(const Class *parent, const type_info &ti)
    : d(new Data(ti))
{
    if(getClass(ti))
        throw Exception(Exception::ItemAlreadyExists);
    d->parent = parent;
#if defined(_MSC_VER)
    parseNameAndNS(ti, d->name, d->ns, d->full_name);
#else
#error "NOT IMPLEMENTED!"
#endif
    Data::classes.insert(this);
}

const Class *Class::parent() const
{
    return d->parent;
}

const String &Class::name() const
{
    return d->name;
}

const String &Class::ns() const
{
    return d->ns;
}

const String &Class::fullName() const
{
    return d->full_name;
}

const type_info &Class::ctype() const
{
    return d->ti;
}

bool Class::isSubclassOf(const Class *cls) const
{
    if (cls == nullptr)
        return false;

    auto type = this;
    do
    {
        if (type == cls)
            return true;
        type = type->parent();
    } while (type);
    return false;
}

Class *Class::getClass(const type_info &ti)
{
    auto iter = std::find_if(
        Data::classes.begin(), Data::classes.end(), [&ti](Class *c)
        { return c->ctype() == ti; });
    if (iter == Data::classes.end())
        return nullptr;
    return *iter;
}

Class *Class::getClass(const String &full_name)
{
    auto iter = std::find_if(
        Data::classes.begin(), Data::classes.end(), [&full_name](Class *c)
        { return c->fullName() == full_name; });
    if (iter == Data::classes.end())
        return nullptr;
    return *iter;
}

bool Class::operator==(const Class &right) const
{
    return d->ti == right.d->ti;
}

bool Class::operator!=(const Class &right) const
{
    return !(*this == right);
}

VINE_CORE_NS_END
