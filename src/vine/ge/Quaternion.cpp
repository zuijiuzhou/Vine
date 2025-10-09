#include <vine/ge/Quaternion.hpp>

VI_GE_NS_BEGIN

#define TMPL_PREFIX template <typename T>

TMPL_PREFIX Quaternion<T>::Quaternion()
  : x(T())
  , y(T())
  , z(T())
  , w(T()) {
}
TMPL_PREFIX Quaternion<T>::Quaternion(T x, T y, T z, T w)
  : x(x)
  , y(y)
  , z(z)
  , w(w) {
}
#undef TMPL_PREFIX

template class VI_GE_API Quaternion<float>;
template class VI_GE_API Quaternion<double>;

VI_GE_NS_END