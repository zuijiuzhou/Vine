#include <vine/math/Line.hpp>

#include <cmath>

#include <vine/math/Math.hpp>


VI_MATH_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX Line<T>::Line(const Point3<T>& origin, const Vector3<T>& direction)
  : origin(origin)
  , direction(direction)
{}

TMPL_PREFIX Line<T>::Intersection Line<T>::intersectWith(const Line<T>& other, T tol) const
{
    Intersection it;

    const Point3<T>& p1 = origin;
    const Vector3<T>& d1 = direction;
    const Point3<T>& p2 = other.origin;
    const Vector3<T>& d2 = other.direction;

    // 原点连线向量
    const Vector3<T> r = p2 - p1;
    // 公共法线
    const Vector3<T> c = d1.cross(d2);

    // 平行
    if (c.length2() < tol * tol)
    {
        it.is_parallel = true;

        // 共线
        if (r.cross(d1).length2() < tol * tol)
        {
            it.is_intersected = true;
            it.is_collinear = true;
        }
        // 平行
        else
        {

        }

        // 已知：d1 与 d2 平行
        // 固定点：p1（在 L1 上）
        // 计算 p1 在 L2 上的最近点
        const auto s = r.dot(d2) / d2.dot(d2);

        it.params.t1 = 0;
        it.params.t2 = s;
        it.params.pt1 = origin;
        it.params.pt2 = p2 + d2 * s;
        it.params.distance = (it.params.pt2 - it.params.pt1).length();

        return it;
    }

    // 非平行
    // 解最近点参数
    const double d1d1 = d1.dot(d1);
    const double d2d2 = d2.dot(d2);
    const double d1d2 = d1.dot(d2);
    const double rd1 = r.dot(d1);
    const double rd2 = r.dot(d2);

    // |d1 × d2|² = |d1|²|d2|² − (d1·d2)²
    const double denom = d1d1 * d2d2 - d1d2 * d1d2;

    if (std::abs(denom) < tol)
    {
        // 平行
        return it;
    }

    it.params.t1 = (rd1 * d2d2 - rd2 * d1d2) / denom;
    it.params.t2 = (rd1 * d1d2 - rd2 * d1d1) / denom;

    it.params.pt1 = p1 + d1 * it.params.t1;
    it.params.pt2 = p2 + d2 * it.params.t2;
    it.params.distance = (it.params.pt1 - it.params.pt2).length();

    // 判断最近点是否重合
    if (it.params.distance < tol)
    {
        it.is_intersected = true;
    }
    //else
    //{
    //    it.params.t1 = 0;
    //    it.params.t2 = 0;
    //    it.params.pt1 = {};
    //    it.params.pt2 = {};
    //}

    return it;
}

TMPL_PREFIX Point3<T> Line<T>::closestPoint(const Point3<T>& pt) const
{
    const auto denom = direction.dot(direction);
    if (denom < EPSD) {
        return origin;
    }

    const Vector3<T> v = pt - origin;
    const double    t = v.dot(direction) / denom;

    return origin + direction * t;
}

template class VI_MATH_API Line<float>;
template class VI_MATH_API Line<double>;

VI_MATH_NS_END