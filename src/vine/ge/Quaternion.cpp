#include <vine/ge/Quaternion.h>
VI_GE_NS_BEGIN
Quaternion ::Quaternion()
  : x(0.)
  , y(0.)
  , z(0.)
  , w(1.) {
}
Quaternion ::Quaternion(double x, double y, double z, double w)
  : x(x)
  , y(y)
  , z(z)
  , w(w) {
}
VI_GE_NS_END