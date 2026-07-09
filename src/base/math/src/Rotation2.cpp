#include <vine/math/Rotation2.hpp>

#include <vine/math/Point2.hpp>
#include <vine/math/Vector2.hpp>

V_MATH_NS_BEGIN

template <typename T>
Point2<T> operator*(const Rotation2<T>& left, const Point2<T>& p)
{
    // p' = R * p = p.x * col0(R) + p.y * col1(R)
    const auto rotated = left.vecs[0] * p.x + left.vecs[1] * p.y;
    return Point2<T>(rotated.x, rotated.y);
}

template <typename T>
Vector2<T> operator*(const Rotation2<T>& left, const Vector2<T>& v)
{
    // v' = R * v = v.x * col0(R) + v.y * col1(R)
    return left.vecs[0] * v.x + left.vecs[1] * v.y;
}

// Explicit template instantiations.
template class V_MATH_API Rotation2<float>;
template class V_MATH_API Rotation2<double>;
template V_MATH_API Point2<float>  operator*(const Rotation2<float>&,  const Point2<float>&);
template V_MATH_API Point2<double> operator*(const Rotation2<double>&, const Point2<double>&);
template V_MATH_API Vector2<float>  operator*(const Rotation2<float>&,  const Vector2<float>&);
template V_MATH_API Vector2<double> operator*(const Rotation2<double>&, const Vector2<double>&);

V_MATH_NS_END
