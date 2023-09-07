#pragma once
#include "core_global.h"

#include <typeinfo>

#include "Std.h"

VI_CORE_NS_BEGIN
class VI_CORE_API Class
{
public:
    Class(const Class *parent, const type_info &ti);
    Class(const Class &cls) = delete;
    Class(const Class &&cls) = delete;
    Class &operator=(const Class &cls) = delete;

    const Class *parent() const;

    const Char *name() const;

    const Char *ns() const;

    const Char *fullName() const;

    bool isSubclassOf(const Class *cls) const;

    const type_info &ctype() const;

public:
    bool operator==(const Class &right) const;
    bool operator!=(const Class &right) const;

public:
    static Class *getClass(const type_info &ti);
    static Class *getClass(const Char *full_name);

private:
    struct Data;
    Data *d;
};

using Type = const Class *;

VI_CORE_NS_END