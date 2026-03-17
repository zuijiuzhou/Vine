#include <vine/math/Plane.hpp>

V_MATH_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX bool Plane<T>::intersectWith(const Line<T>& line, Point3<T>& intersection_pt, T eps) const
{
    // Plane-Line intersection: find the point where a line intersects a plane.
    //
    // Plane equation: normal · (P - origin) = 0
    // Line parametric form: P(t) = line.origin + t * line.direction
    //
    // Substituting the line into the plane equation:
    // normal · (line.origin + t * line.direction - origin) = 0
    // normal · (line.origin - origin) + t * (normal · line.direction) = 0
    //
    // Solving for t: denom = normal · line.direction
    // If |denom| ≤ eps: line is parallel to plane (no intersection or infinite intersections)
    // Else: t = -normal · (line.origin - origin) / denom

    Vector3<T> diff  = line.origin - origin;
    T          denom = normal.dot(line.direction);

    // Check if line is parallel to plane
    if (isZero(denom, eps)) {
        return false;
    }

    T numer         = -normal.dot(diff);
    T t             = numer / denom;
    intersection_pt = line.origin + line.direction * t;

    return true;
}

TMPL_PREFIX
bool Plane<T>::intersectWith(const Plane<T>& other, Line<T>& intersection_line, T eps) const
{
    // Plane-Plane intersection: find the line where two planes intersect.
    //
    // Plane 1 equation: normal · (P - origin) = 0      => normal · P = d1, where d1 = normal · origin
    // Plane 2 equation: other.normal · (P - other.origin) = 0  => other.normal · P = d2
    //
    // The intersection line has:
    //   - Direction: normal × other.normal (perpendicular to both normals)
    //   - A point lying on both planes (found by solving a 2x2 linear system)
    //
    // Strategy: Choose which coordinate to fix based on which direction component is largest.
    // This ensures the 2x2 matrix has the best numerical properties.

    Vector3<T> dir = normal.cross(other.normal);

    // Check if planes are parallel or coincident
    if (isZero(dir.length(), eps)) {
        return false;
    }

    // Determine which coordinate to fix based on the largest component of direction
    int fix_axis = 0;
    T   abs_x    = std::abs(dir.x);
    T   abs_y    = std::abs(dir.y);
    T   abs_z    = std::abs(dir.z);

    if (abs_y > abs_x && abs_y > abs_z) {
        fix_axis = 1;
    }
    else if (abs_z > abs_x && abs_z > abs_y) {
        fix_axis = 2;
    }

    // Compute plane constants: d1 = normal · origin, d2 = other.normal · other.origin
    T d1 = normal.dot(Vector3<T>{ origin.x, origin.y, origin.z });
    T d2 = other.normal.dot(Vector3<T>{ other.origin.x, other.origin.y, other.origin.z });

    // Lambda to solve a 2x2 system and set pt coordinates
    // Solves: m11*u + m12*v = d1, m21*u + m22*v = d2 => u, v in pt
    auto solve_2x2 = [eps](T m11, T m12, T m21, T m22, T d1, T d2, T& u, T& v) -> bool {
        T det = m11 * m22 - m12 * m21;
        if (isZero(det, eps)) {
            return false;
        }
        u = (d1 * m22 - d2 * m12) / det;
        v = (m11 * d2 - m21 * d1) / det;
        return true;
    };

    Point3<T> pt;

    if (fix_axis == 0) {
        // Fix x=0, solve for y,z
        pt.x = T(0);
        if (!solve_2x2(normal.y, normal.z, other.normal.y, other.normal.z, d1, d2, pt.y, pt.z)) {
            // Fallback: fix y=0, solve for x,z
            pt.y = T(0);
            solve_2x2(normal.x, normal.z, other.normal.x, other.normal.z, d1, d2, pt.x, pt.z);
        }
    }
    else if (fix_axis == 1) {
        // Fix y=0, solve for x,z
        pt.y = T(0);
        if (!solve_2x2(normal.x, normal.z, other.normal.x, other.normal.z, d1, d2, pt.x, pt.z)) {
            // Fallback: fix z=0, solve for x,y
            pt.z = T(0);
            solve_2x2(normal.x, normal.y, other.normal.x, other.normal.y, d1, d2, pt.x, pt.y);
        }
    }
    else {
        // Fix z=0, solve for x,y
        pt.z = T(0);
        if (!solve_2x2(normal.x, normal.y, other.normal.x, other.normal.y, d1, d2, pt.x, pt.y)) {
            // Fallback: fix x=0, solve for y,z
            pt.x = T(0);
            solve_2x2(normal.y, normal.z, other.normal.y, other.normal.z, d1, d2, pt.y, pt.z);
        }
    }

    intersection_line.origin    = pt;
    intersection_line.direction = dir;

    return true;
}

template class V_MATH_API Plane<float>;
template class V_MATH_API Plane<double>;
V_MATH_NS_END
