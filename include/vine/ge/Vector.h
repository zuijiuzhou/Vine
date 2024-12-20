#pragma once

#include "Ge.h"
#include "ge_global.h"

VI_GE_NS_BEGIN
template <typename T, unsigned int N> class Vector {
  public:
    using ValType                     = T;
    using VecType                     = Vector<T, N>;
    static constexpr unsigned int DIM = N;

  public:
    Vector()
      : data{ 0 } {}

  public:
    double dotProduct(const VecType& vec) const {
      double d = 0;
      return 0;
    }

    void set(unsigned int n, T val) { data[n] = val; }

  public:
    T data[N];
};
VI_GE_NS_END