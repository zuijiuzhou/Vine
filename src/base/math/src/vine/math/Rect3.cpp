#include <vine/math/Rect3.hpp>

#include <algorithm>
#include <cstdint>

#include <vine/math/Math.hpp>
#include <vine/math/Point3.hpp>
#include <vine/math/Vector3.hpp>


VI_MATH_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX bool Rect3<T>::contains(T x, T y, T z) const
{
    auto l = lowerBound();
    auto u = upperBound();
    return x >= l.x && x <= u.x && y >= l.y && y <= u.y && z >= l.z && z <= u.z;
}

TMPL_PREFIX bool Rect3<T>::contains(const Point3<T>& pt) const
{
    auto l = lowerBound();
    auto u = upperBound();
    return pt.x >= l.x && pt.x <= u.x && pt.y >= l.y && pt.y <= u.y && pt.z >= l.z && pt.z <= u.z;
}

TMPL_PREFIX void Rect3<T>::expandBy(const Point3<T>& pt)
{
    auto lb = lowerBound();
    auto ub = upperBound();
    lb.x    = std::min<T>(lb.x, pt.x);
    lb.y    = std::min<T>(lb.y, pt.y);
    lb.z    = std::min<T>(lb.z, pt.z);
    ub.x    = std::max<T>(ub.x, pt.x);
    ub.y    = std::max<T>(ub.y, pt.y);
    ub.z    = std::max<T>(ub.z, pt.z);
    x       = lb.x;
    y       = lb.y;
    z       = lb.z;
    l       = ub.x - lb.x;
    w       = ub.y - lb.y;
    h       = ub.z - lb.z;
}

TMPL_PREFIX void Rect3<T>::expandBy(const Rect3<T>& rect)
{
    auto lb1 = lowerBound();
    auto ub1 = upperBound();
    auto lb2 = rect.lowerBound();
    auto ub2 = rect.upperBound();

    lb1.x = std::min<T>(lb1.x, lb2.x);
    lb1.y = std::min<T>(lb1.y, lb2.y);
    lb1.z = std::min<T>(lb1.z, lb2.z);
    ub1.x = std::max<T>(ub1.x, ub2.x);
    ub1.y = std::max<T>(ub1.y, ub2.y);
    ub1.z = std::max<T>(ub1.z, ub2.z);

    x = lb1.x;
    y = lb1.y;
    z = lb1.z;

    l = ub1.x - lb1.x;
    w = ub1.y - lb1.y;
    h = ub1.z - lb1.z;
}

TMPL_PREFIX Rect3<T> Rect3<T>::intersectWith(const Rect3<T>& rect) const
{
    return Rect3<T>();
}

#undef TMPL_PREFIX

template class VI_MATH_API Rect3<float>;
template class VI_MATH_API Rect3<double>;
template class VI_MATH_API Rect3<int8_t>;
template class VI_MATH_API Rect3<uint8_t>;
template class VI_MATH_API Rect3<int16_t>;
template class VI_MATH_API Rect3<uint16_t>;
template class VI_MATH_API Rect3<int32_t>;
template class VI_MATH_API Rect3<uint32_t>;
template class VI_MATH_API Rect3<int64_t>;
template class VI_MATH_API Rect3<uint64_t>;

VI_MATH_NS_END
