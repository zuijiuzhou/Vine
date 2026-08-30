#pragma once

#include "geometry_global.hpp"

#include "Shape.hpp"

V_GEOMETRY_NS_BEGIN

/**
 * @brief Base class for parametric primitive shapes.
 *
 * Concrete primitives (Box, Cylinder, Cone, Sphere, Ellipsoid) derive from
 * this class and describe their geometry with a small set of dimensions.
 */
class V_GEOMETRY_API Primitive : public Shape {
    V_OBJECT_META_DECL;

  protected:
    /// Protected so Primitive cannot be instantiated directly.
    Primitive();
};

V_GEOMETRY_NS_END