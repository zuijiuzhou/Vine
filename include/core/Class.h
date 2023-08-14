#pragma once

#include <type_traits>

#include "core_global.h"
#include "String.h"

VINE_CORE_NS_BEGIN
class VINE_CORE_API Class
{
public:
    Class(const Class *parent, const type_info& ti);
    Class(const Class& cls) = delete;
    Class(const Class&& cls) = delete;
    Class& operator = (const Class& cls) = delete;

    const Class* parent() const;

    const String& name() const;

    const String& ns() const;

    const String& fullName() const;

    bool isSubclassOf(const Class* cls) const;
private:
    struct Data;
    Data *d;
};

using Type = const Class*;

VINE_CORE_NS_END