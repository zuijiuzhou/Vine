#pragma once

#include "geometry_global.hpp"

#include "Primitive.hpp"

V_GEOMETRY_NS_BEGIN

/**
 * @brief A sphere primitive centered at the origin.
 */
class V_GEOMETRY_API Sphere : public Primitive {
    V_OBJECT_META_DECL;

  public:
    /**
     * @brief Constructs a unit sphere (radius 0.5).
     */
    Sphere();

    /**
     * @brief Constructs a sphere from its radius.
     *
     * @param radius Radius of the sphere.
     */
    explicit Sphere(double radius);

  public:
    /**
     * @brief Returns the radius of the sphere.
     *
     * @return Radius.
     */
    double radius() const;

    /**
     * @brief Sets the radius of the sphere.
     *
     * @param radius New radius.
     */
    void setRadius(double radius);

    [[nodiscard]]
    bool isValid() const override;

    /**
     * @brief Returns whether the sphere encloses a volume.
     *
     * @param eps Numerical tolerance for the radius.
     * @return true when the radius is larger than eps.
     */
    [[nodiscard]]
    bool hasVolume(double eps = vine::math::EPS<double>()) const override;

  private:
    /// Radius of the sphere.
    double radius_ = 0.5;
};

V_GEOMETRY_NS_END
