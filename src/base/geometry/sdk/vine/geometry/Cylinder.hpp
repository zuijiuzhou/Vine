#pragma once

#include "geometry_global.hpp"

#include "Primitive.hpp"

V_GEOMETRY_NS_BEGIN

/**
 * @brief A cylinder primitive centered on the Z axis.
 */
class V_GEOMETRY_API Cylinder : public Primitive {
    V_OBJECT_META_DECL;

  public:
    /**
     * @brief Constructs a unit cylinder (radius 0.5, height 1.0).
     */
    Cylinder();

    /**
     * @brief Constructs a cylinder from its dimensions.
     *
     * @param radius Radius of the circular cross-section.
     * @param height Extent along the Z axis.
     */
    Cylinder(double radius, double height);

  public:
    /**
     * @brief Returns the radius of the circular cross-section.
     *
     * @return Radius.
     */
    double radius() const;

    /**
     * @brief Sets the radius of the circular cross-section.
     *
     * @param radius New radius.
     */
    void setRadius(double radius);

    /**
     * @brief Returns the extent along the Z axis.
     *
     * @return Height.
     */
    double height() const;

    /**
     * @brief Sets the extent along the Z axis.
     *
     * @param height New height.
     */
    void setHeight(double height);

    [[nodiscard]]
    bool isValid() const override;

    /**
     * @brief Returns whether the cylinder encloses a volume.
     *
     * @param eps Numerical tolerance for the dimensions.
     * @return true when both radius and height are larger than eps.
     */
    [[nodiscard]]
    bool hasVolume(double eps = vine::math::EPS<double>()) const override;

  private:
    /// Radius of the circular cross-section.
    double radius_ = 0.5;
    /// Extent along the Z axis.
    double height_ = 1.0;
};

V_GEOMETRY_NS_END
