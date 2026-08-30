#pragma once

#include "core_global.hpp"

#include <type_traits>

#include "Object.hpp"

V_CORE_NS_BEGIN

/**
 * @brief Interface for objects that can create a copy of themselves.
 *
 * The returned clone is owned by the caller.
 */
class ICloneable {
    V_DECLARE_INTERFACE(ICloneable)

  public:
    virtual ~ICloneable() = default;

    /**
     * @brief Creates a copy of the object.
     *
     * @return A new Object owned by the caller.
     */
    virtual Object* clone() const = 0;
};

/**
 * @brief Concept for types that derive from ICloneable.
 */
template <typename T>
concept Cloneable = std::is_base_of_v<ICloneable, T>;

V_CORE_NS_END
