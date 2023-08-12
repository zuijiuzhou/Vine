#include <core/Class.h>

#include <typeinfo>

VINE_CORE_NS_BEGIN

namespace{
    bool parseNameAndNS(const type_info& ti, String& name, String& ns, String& full_name){
        auto n = ti.name();
        auto rn = ti.raw_name();
        full_name = String::fromUtf8(n);
        full_name = full_name.substr(6);
        
        return true;
    }
}

struct Class::Data
{
    const Class *parent = nullptr;
    const type_info* ti;
    String name;
    String ns;
    String full_name;
};

Class::Class(const Class *parent, const type_info& ti)
    : d(new Data)
{
    d->parent = parent;
    d->ti = &ti;
#if defined(_MSC_VER)
    parseNameAndNS(ti, d->name, d->name, d->full_name);
#else
#error "NOT IMPLEMENTED!"
#endif
}

const Class *Class::parent() const
{
    return d->parent;
}

const String& Class::name() const{
    return d->name;
}

const String& Class::ns() const{
    return d->ns;
}

const String& Class::fullName() const{
    return d->full_name;
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

VINE_CORE_NS_END
