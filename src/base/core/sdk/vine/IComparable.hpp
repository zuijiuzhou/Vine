#pragma once

#include "core_global.hpp"

#include <type_traits>

#include "Object.hpp"

V_CORE_NS_BEGIN

/**
 * @brief Interface for objects that can be ordered against another Object.
 */
class IComparable {
    V_DECLARE_INTERFACE(IComparable)

  public:
    virtual ~IComparable() = default;

    /**
     * @brief Compares this object with other.
     *
     * @param other The object to compare against.
     * @return A negative value when less, zero when equal, positive when
     *         greater.
     */
    virtual int compareTo(const Object& other) const = 0;
};

/**
 * @brief Concept for types that derive from IComparable.
 */
template <typename T>
concept Comparable = std::is_base_of_v<IComparable, T>;

V_CORE_NS_END
