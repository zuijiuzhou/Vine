#pragma once

#include <type_traits>

#include "core_global.h"

VINE_CORE_NS_BEGIN
class Object;
class VINE_CORE_API Class
{
public:
    Class(const Class *parent);

    const Class* parent() const;

    bool isSubclassOf(const Class* cls) const;
private:
    struct Data;
    Data *d;
};
VINE_CORE_NS_END