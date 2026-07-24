#include <vine/math/Vector3.hpp>

V_MATH_NS_BEGIN

#define TMPL_PREFIX template <typename T>

#undef TMPL_PREFIX

template class V_MATH_API Vector3<float>;
template class V_MATH_API Vector3<double>;
template class V_MATH_API Vector3<bool>;
template class V_MATH_API Vector3<int8_t>;
template class V_MATH_API Vector3<uint8_t>;
template class V_MATH_API Vector3<int16_t>;
template class V_MATH_API Vector3<uint16_t>;
template class V_MATH_API Vector3<int32_t>;
template class V_MATH_API Vector3<uint32_t>;
template class V_MATH_API Vector3<int64_t>;
template class V_MATH_API Vector3<uint64_t>;

V_MATH_NS_END
