#pragma once

#include "core_global.hpp"

#include <concepts>

#include "String.hpp"
#include "Type.hpp"

V_CORE_NS_BEGIN

/**
 * @brief Interface for objects that can render themselves as a String.
 */
class IStringable {
    V_DECLARE_INTERFACE(IStringable)

  public:
    virtual ~IStringable() = default;

    /**
     * @brief Renders the object as a String.
     *
     * @return The string representation.
     */
    virtual String toString() const = 0;
};

/**
 * @brief Concept for types that provide a toString() method.
 */
template <typename T>
concept Stringable = requires(const T& t) {
    { t.toString() } -> std::convertible_to<String>;
};

V_CORE_NS_END
