#pragma once

#include "geometry_global.hpp"

#include "Shape.hpp"

V_GEOMETRY_NS_BEGIN

/**
 * @brief Base class for polygonal mesh shapes.
 */
class V_GEOMETRY_API Mesh : public Shape {
    V_OBJECT_META_DECL;

  protected:
    /// Protected so Mesh cannot be instantiated directly.
    Mesh();
};

V_GEOMETRY_NS_END
