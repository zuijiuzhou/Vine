#include <vine/math/Line.hpp>

VI_MATH_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX Line<T>::Line(const Point3<T>& origin, const Vector3<T>& direction)
  : origin(origin)
  , direction(direction)
{}

TMPL_PREFIX Line<T>::Intersection Line<T>::intersectWith(const Line<T>& line, T tol) const
{
    Intersection it;

    // const VVector3d& p1 = origin;
    // const VVector3d& d1 = direction;
    // const VVector3d& p2 = other.origin;
    // const VVector3d& d2 = other.direction;

    // // 原点连线向量
    // const VVector3d r = p2 - p1;
    // // 公共法线
    // const VVector3d c = d1.cross(d2);

    // // 平行
    // if (c.squaredNorm() < eps * eps) {
    //     // 共线
    //     if (r.cross(d1).squaredNorm() < eps * eps) {
    //         it.valid      = true;
    //         it.coincident = true;
    //     }

    //     return it;
    // }

    // // 非平行
    // // 解最近点参数
    // const double d1d1 = d1.dot(d1);
    // const double d2d2 = d2.dot(d2);
    // const double d1d2 = d1.dot(d2);
    // const double rd1  = r.dot(d1);
    // const double rd2  = r.dot(d2);

    // const double denom = d1d1 * d2d2 - d1d2 * d1d2;

    // if (std::abs(denom) < eps) {
    //     return it;
    // }

    // it.t1 = (rd1 * d2d2 - rd2 * d1d2) / denom;
    // it.t2 = (rd1 * d1d2 - rd2 * d1d1) / denom;

    // const auto q1 = p1 + it.t1 * d1;
    // const auto q2 = p2 + it.t2 * d2;

    // // 判断最近点是否重合
    // if ((q1 - q2).norm() < eps) {
    //     it.valid = true;
    // }

    // it.t1 = 0;
    // it.t2 = 0;

    return it;
}

template class VI_MATH_API Line<float>;
template class VI_MATH_API Line<double>;

VI_MATH_NS_END