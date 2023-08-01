#pragma once

#include <type_traits>

#include "core_export.h"

ETD_CORE_NS_BEGIN
class Object;
class ETD_CORE_API Class
{
public:
    Class(const Class *parent);

    const Class* parent() const;

    bool isSubclassOf(const Class* cls) const;
private:
    struct Data;
    Data *d;
};
ETD_CORE_NS_END