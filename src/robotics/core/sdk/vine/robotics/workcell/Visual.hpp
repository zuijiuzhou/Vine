#pragma once

#include <vine/robotics/robot_core_global.hpp>

#include <vine/IntrusivePtr.hpp>
#include <vine/math/Isometry3.hpp>

#include <vine/geometry/Material.hpp>
#include <vine/geometry/Shape.hpp>

V_ROBOTICS_WORKCELL_NS_BEGIN

/**
 * @brief Visual geometry: shape, transform and surface material.
 *
 * Used for rendering. Holds intrusive pointers to a shared shape and a shared
 * material; the owning scene/model keeps them alive, so a visual only borrows
 * them. A value type owned by the rigid body it belongs to.
 */
class Visual
{
  public:
    /**
     * @brief Returns the shape to render.
     *
     * @return The shape, or null when unset.
     */
    const vine::IntrusivePtr<vine::geometry::Shape>& shape() const
    {
        return shape_;
    }

    /**
     * @brief Sets the shape to render.
     *
     * @param shape New shape.
     */
    void setShape(const vine::IntrusivePtr<vine::geometry::Shape>& shape)
    {
        shape_ = shape;
    }

    /**
     * @brief Returns the local transform of this visual.
     *
     * @return The transform relative to the body root frame.
     */
    const math::Isometry3d& tf() const
    {
        return tf_;
    }

    /**
     * @brief Sets the local transform of this visual.
     *
     * @param tf New transform relative to the body root frame.
     */
    void setTf(const math::Isometry3d& tf)
    {
        tf_ = tf;
    }

    /**
     * @brief Returns the surface material.
     *
     * @return The material, or null when unset.
     */
    const vine::IntrusivePtr<vine::geometry::Material>& material() const
    {
        return material_;
    }

    /**
     * @brief Sets the surface material.
     *
     * @param material New material.
     */
    void setMaterial(const vine::IntrusivePtr<vine::geometry::Material>& material)
    {
        material_ = material;
    }

  private:
    vine::IntrusivePtr<vine::geometry::Shape>    shape_;
    math::Isometry3d                             tf_;
    vine::IntrusivePtr<vine::geometry::Material> material_;
};

V_ROBOTICS_WORKCELL_NS_END
