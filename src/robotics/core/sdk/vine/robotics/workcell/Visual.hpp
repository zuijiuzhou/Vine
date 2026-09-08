#pragma once

#include <vine/robotics/robot_core_global.hpp>

#include <vine/intrusive_ptr.hpp>
#include <vine/math/Isometry3.hpp>
#include <vine/String.hpp>

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
    const vine::intrusive_ptr<vine::geometry::Shape>& shape() const
    {
        return shape_;
    }

    /**
     * @brief Sets the shape to render.
     *
     * @param shape New shape.
     */
    void setShape(const vine::intrusive_ptr<vine::geometry::Shape>& shape)
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
    const vine::intrusive_ptr<vine::geometry::Material>& material() const
    {
        return material_;
    }

    /**
     * @brief Sets the surface material.
     *
     * @param material New material.
     */
    void setMaterial(const vine::intrusive_ptr<vine::geometry::Material>& material)
    {
        material_ = material;
    }

    /**
     * @brief Returns the device material name this visual references.
     *
     * @return The material name, or empty when the material is inline.
     */
    const String& materialName() const
    {
        return material_name_;
    }

    /**
     * @brief Sets the device material name this visual references.
     *
     * @param name The device material name.
     */
    void setMaterialName(const String& name)
    {
        material_name_ = name;
    }

  private:
    vine::intrusive_ptr<vine::geometry::Shape>    shape_;
    math::Isometry3d                             tf_;
    vine::intrusive_ptr<vine::geometry::Material> material_;
    String                                       material_name_;
};

V_ROBOTICS_WORKCELL_NS_END
