#pragma once
#include "graphics_global.hpp"
#include "Ray.hpp"

#include <vine/math/Vector3.hpp>
#include <vine/math/Vector2.hpp>
#include <vine/math/Matrix4x4.hpp>
#include <vine/intrusive_ptr.hpp>
#include <vector>
#include <limits>

V_GRAPHICS_NS_BEGIN

using vine::math::Vec3d;
using vine::math::Vec2d;
using vine::math::Mat4d;

class Geometry;
class Scene;

/**
 * @brief Result of ray-geometry intersection test.
 *
 * Contains detailed information about the intersection point, if any.
 */
struct V_GRAPHICS_API RayIntersectionResult {
    /** Whether the ray hit the geometry. */
    bool hit = false;

    /** Intersection point position (world space). */
    Vec3d point;

    /** Surface normal at intersection point. */
    Vec3d normal;

    /** Distance from ray origin to intersection point. */
    double distance = std::numeric_limits<double>::max();

    /** Intersected geometry. */
    intrusive_ptr<Geometry> geometry;

    /** Triangle index (if applicable, otherwise max). */
    std::size_t triangleIndex = std::numeric_limits<std::size_t>::max();
};

/**
 * @brief Ray-geometry intersection computation.
 *
 * Provides static methods for computing ray-geometry intersections,
 * supporting both single geometry and full scene queries.
 */
class V_GRAPHICS_API RayIntersection {
  public:
    /** @brief Tests ray intersection with a single geometry.
     *
     * @param ray      The ray to test.
     * @param geometry The geometry to intersect with.
     * @param world    World transform of the geometry.
     * @return Intersection result. hit=false if no intersection.
     */
    static RayIntersectionResult intersect(const Ray& ray, Geometry* geometry,
                                           const Mat4d& world);

    /** @brief Tests ray intersection with all geometries in a scene.
     *
     * Returns only the closest intersection.
     *
     * @param ray   The ray to test.
     * @param scene The scene containing geometries.
     * @return Intersection result. hit=false if no intersection.
     */
    static RayIntersectionResult intersectScene(const Ray& ray, Scene* scene);

    /** @brief Tests ray intersection with all geometries in a scene.
     *
     * Returns all intersections sorted by distance.
     *
     * @param ray   The ray to test.
     * @param scene The scene containing geometries.
     * @return Vector of intersection results (may be empty).
     */
    static std::vector<RayIntersectionResult> intersectSceneAll(const Ray& ray,
                                                                 Scene* scene);
};

V_GRAPHICS_NS_END
