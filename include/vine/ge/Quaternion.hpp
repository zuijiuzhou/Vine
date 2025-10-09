#pragma once

#include "ge_global.hpp"

#include <cstdint>

VI_GE_NS_BEGIN

template <typename T> class Quaternion {
  public:
    Quaternion();
    Quaternion(T x, T y, T z, T w);

  private:
    T x, y, z, w;
};

using Quatf = Quaternion<float>;
using Quatd = Quaternion<double>;

VI_GE_NS_END