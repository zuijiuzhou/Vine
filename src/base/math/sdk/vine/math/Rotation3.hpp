#pragma once

#include "math_global.hpp"

#include "Quaternion3.hpp"

V_MATH_NS_BEGIN
template <typename T>
class Point3;
template <typename T>
class Vector3;

/**
 * @brief 3D rigid transformation class that represents translation, rotation.
 * @tparam T Only accepts float and double
 */
template <typename T>
class Rotation3 {
  public:
    using value_type = T;

  public:
    /**
     * @brief Construct an identity rotation.
     */
    constexpr Rotation3() noexcept
      : data{T(1), T(0), T(0), T(0), T(1), T(0), T(0), T(0), T(1)}
    {}

    /**
     * @brief Construct a rotation from a quaternion.
     * @param quat Rotation quaternion.
     */
    Rotation3(const Quaternion3<T>& quat) noexcept;

  public:
    /**
     * @brief Convert this rotation to a quaternion.
     * @return Quaternion representing the same rotation.
     */
    Quaternion3<T> toQuaternion() const;

    /**
     * @brief Set this rotation from a quaternion.
     * @param quat Rotation quaternion.
     */
    void fromQuaternion(const Quaternion3<T>& quat);

  public:
    union
    {
        struct {
            // row0 row1 row2
            T m00, m10, m20; // col 0
            T m01, m11, m21; // col 1
            T m02, m12, m22; // col 2
        };

        struct {
            Vector3<T> vec0;
            Vector3<T> vec1;
            Vector3<T> vec2;
        };

        Vector3<T> vecs[3];

        T data[9];
    };
};

V_MATH_NS_END
