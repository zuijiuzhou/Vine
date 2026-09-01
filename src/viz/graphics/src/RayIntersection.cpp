#include <vine/graphics/RayIntersection.hpp>

#include <vine/graphics/Geometry.hpp>
#include <vine/graphics/Node.hpp>
#include <vine/graphics/Scene.hpp>
#include <vine/geometry/IndexedTriangleMesh.hpp>
#include <vine/geometry/TriangleMesh.hpp>
#include <vine/math/Transform3.hpp>

#include <algorithm>

V_GRAPHICS_NS_BEGIN

namespace
{

/**
 * @brief Ray-triangle intersection using the Möller–Trumbore algorithm.
 *
 * @param ray        The ray.
 * @param a          Triangle vertex A (world space).
 * @param b          Triangle vertex B (world space).
 * @param c          Triangle vertex C (world space).
 * @param out_t      Receives the hit distance when intersecting.
 * @return true when the ray hits the triangle.
 */
bool intersectTriangle(const Ray& ray, const Vec3d& a, const Vec3d& b, const Vec3d& c,
                       double& out_t)
{
    constexpr double eps = 1e-9;
    const Vec3d edge1 = b - a;
    const Vec3d edge2 = c - a;
    const Vec3d h = ray.direction.cross(edge2);
    const double det = edge1.dot(h);
    if (std::abs(det) < eps) {
        return false;
    }
    const double inv_det = 1.0 / det;
    const Vec3d s = ray.origin - a;
    const double u = s.dot(h) * inv_det;
    if (u < 0.0 || u > 1.0) {
        return false;
    }
    const Vec3d q = s.cross(edge1);
    const double v = ray.direction.dot(q) * inv_det;
    if (v < 0.0 || u + v > 1.0) {
        return false;
    }
    const double t = edge2.dot(q) * inv_det;
    if (t > eps) {
        out_t = t;
        return true;
    }
    return false;
}

/**
 * @brief Computes the normal of a triangle.
 *
 * @param a Triangle vertex A.
 * @param b Triangle vertex B.
 * @param c Triangle vertex C.
 * @return Face normal (unit length).
 */
Vec3d faceNormal(const Vec3d& a, const Vec3d& b, const Vec3d& c)
{
    return (b - a).cross(c - a).normalized();
}

/**
 * @brief Recursively tests a ray against geometry drawables in a node subtree.
 *
 * @param ray   The ray.
 * @param node  Node to traverse.
 * @param best  Closest intersection found so far (in/out).
 */
void intersectNode(const Ray& ray, const Node* node, RayIntersectionResult& best)
{
    if (node == nullptr || !node->isVisible()) {
        return;
    }
    for (const auto& drawable : node->drawables()) {
        if (Geometry* geom = dynamic_cast<Geometry*>(drawable.get())) {
            RayIntersectionResult r = RayIntersection::intersect(ray, geom, node->worldTransform());
            if (r.hit && r.distance < best.distance) {
                best = r;
            }
        }
    }
    for (const auto& child : node->children()) {
        intersectNode(ray, child.get(), best);
    }
}

/**
 * @brief Recursively collects all ray-geometry intersections in a node subtree.
 *
 * @param ray   The ray.
 * @param node  Node to traverse.
 * @param out   Output result list.
 */
void intersectNodeAll(const Ray& ray, const Node* node, std::vector<RayIntersectionResult>& out)
{
    if (node == nullptr || !node->isVisible()) {
        return;
    }
    for (const auto& drawable : node->drawables()) {
        if (Geometry* geom = dynamic_cast<Geometry*>(drawable.get())) {
            RayIntersectionResult r = RayIntersection::intersect(ray, geom, node->worldTransform());
            if (r.hit) {
                out.push_back(r);
            }
        }
    }
    for (const auto& child : node->children()) {
        intersectNodeAll(ray, child.get(), out);
    }
}

}  // namespace

RayIntersectionResult RayIntersection::intersect(const Ray& ray, Geometry* geometry,
                                                 const Mat4d& world)
{
    RayIntersectionResult result;
    if (geometry == nullptr) {
        return result;
    }
    const auto* shape = geometry->shape();
    if (shape == nullptr) {
        return result;
    }
    double best_t = std::numeric_limits<double>::max();

    switch (shape->shapeType()) {
        case vine::geometry::ShapeType::TriangleMesh: {
            const auto* mesh = dynamic_cast<const vine::geometry::TriangleMesh*>(shape);
            if (mesh == nullptr) {
                return result;
            }
            const auto& positions = mesh->positions();
            for (std::size_t i = 0; i + 2 < positions.size(); i += 3) {
                const Vec3d a = world * Vec3d(positions[i].x, positions[i].y, positions[i].z);
                const Vec3d b = world * Vec3d(positions[i + 1].x, positions[i + 1].y, positions[i + 1].z);
                const Vec3d c = world * Vec3d(positions[i + 2].x, positions[i + 2].y, positions[i + 2].z);
                double t = 0.0;
                if (intersectTriangle(ray, a, b, c, t) && t < best_t) {
                    best_t = t;
                    result.hit = true;
                    result.point = ray.pointAt(t);
                    result.normal = faceNormal(a, b, c);
                    result.distance = t;
                    result.triangleIndex = i / 3;
                }
            }
            break;
        }
        case vine::geometry::ShapeType::IndexedTriangleMesh: {
            const auto* mesh = dynamic_cast<const vine::geometry::IndexedTriangleMesh*>(shape);
            if (mesh == nullptr) {
                return result;
            }
            const auto& positions = mesh->positions();
            const auto& indices = mesh->indices();
            for (std::size_t tri = 0; tri + 2 < indices.size(); tri += 3) {
                const auto& ia = indices[tri];
                const auto& ib = indices[tri + 1];
                const auto& ic = indices[tri + 2];
                if (ia >= positions.size() || ib >= positions.size() || ic >= positions.size()) {
                    continue;
                }
                const Vec3d a = world * Vec3d(positions[ia].x, positions[ia].y, positions[ia].z);
                const Vec3d b = world * Vec3d(positions[ib].x, positions[ib].y, positions[ib].z);
                const Vec3d c = world * Vec3d(positions[ic].x, positions[ic].y, positions[ic].z);
                double t = 0.0;
                if (intersectTriangle(ray, a, b, c, t) && t < best_t) {
                    best_t = t;
                    result.hit = true;
                    result.point = ray.pointAt(t);
                    result.normal = faceNormal(a, b, c);
                    result.distance = t;
                    result.triangleIndex = tri / 3;
                }
            }
            break;
        }
        default:
            break;
    }

    if (result.hit) {
        result.geometry = geometry;
    }
    return result;
}

RayIntersectionResult RayIntersection::intersectScene(const Ray& ray, Scene* scene)
{
    RayIntersectionResult best;
    if (scene == nullptr) {
        return best;
    }
    for (const auto& node : scene->nodes()) {
        intersectNode(ray, node.get(), best);
    }
    return best;
}

std::vector<RayIntersectionResult> RayIntersection::intersectSceneAll(const Ray& ray, Scene* scene)
{
    std::vector<RayIntersectionResult> results;
    if (scene == nullptr) {
        return results;
    }
    for (const auto& node : scene->nodes()) {
        intersectNodeAll(ray, node.get(), results);
    }
    std::sort(results.begin(), results.end(),
              [](const RayIntersectionResult& lhs, const RayIntersectionResult& rhs) {
                  return lhs.distance < rhs.distance;
              });
    return results;
}

V_GRAPHICS_NS_END
