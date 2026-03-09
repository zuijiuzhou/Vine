#include <vine/math/Point3.hpp>

#include <cassert>

#include <vine/math/Math.hpp>
#include <vine/math/Point2.hpp>
#include <vine/math/Vector3.hpp>

VI_MATH_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX Vector3<T> Point3<T>::toVector() const
{
    return Vector3<T>(x, y, z);
}

#undef TMPL_PREFIX

template class VI_MATH_API Point3<float>;
template class VI_MATH_API Point3<double>;
template class VI_MATH_API Point3<bool>;
template class VI_MATH_API Point3<int8_t>;
template class VI_MATH_API Point3<uint8_t>;
template class VI_MATH_API Point3<int16_t>;
template class VI_MATH_API Point3<uint16_t>;
template class VI_MATH_API Point3<int32_t>;
template class VI_MATH_API Point3<uint32_t>;
template class VI_MATH_API Point3<int64_t>;
template class VI_MATH_API Point3<uint64_t>;

VI_MATH_NS_END
