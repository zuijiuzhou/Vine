#pragma once

#include "core_global.hpp"

#include <concepts>

#include "String.hpp"
#include "Type.hpp"

V_CORE_NS_BEGIN

/**
 * @brief Interface for objects that can render themselves as a String.
 */
class INameable {
    V_DECLARE_INTERFACE(INameable)

  public:
    virtual ~INameable() = default;
    
    /**
     * @brief Returns the object name.
     *
     * @return The name.
     */
    virtual const String& name() const = 0;

    /**
     * @brief Sets the object name.
     *
     * @param name The new name.
     */
    virtual void setName(const String& name) = 0;
};

/**
 * @brief Concept for types that provide name() and setName().
 */
template <typename T>
concept Nameable = requires(T& t, const String& name) {
    { t.name() } -> std::convertible_to<const String&>;
    t.setName(name);
};

V_CORE_NS_END
