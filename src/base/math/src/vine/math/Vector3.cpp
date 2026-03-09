#include <vine/math/Vector3.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <type_traits>

#include <vine/math/Math.hpp>
#include <vine/math/Point3.hpp>
#include <vine/math/Vector2.hpp>

VI_MATH_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX Point3<T> Vector3<T>::toPoint() const
{
    return Point3<T>(x, y, z);
}

#undef TMPL_PREFIX

template class VI_MATH_API Vector3<float>;
template class VI_MATH_API Vector3<double>;
template class VI_MATH_API Vector3<bool>;
template class VI_MATH_API Vector3<int8_t>;
template class VI_MATH_API Vector3<uint8_t>;
template class VI_MATH_API Vector3<int16_t>;
template class VI_MATH_API Vector3<uint16_t>;
template class VI_MATH_API Vector3<int32_t>;
template class VI_MATH_API Vector3<uint32_t>;
template class VI_MATH_API Vector3<int64_t>;
template class VI_MATH_API Vector3<uint64_t>;

VI_MATH_NS_END
