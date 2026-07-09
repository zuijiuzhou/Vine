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
      : data{ T(1), T(0), T(0), T(0), T(1), T(0), T(0), T(0), T(1) }
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

    /**
     * @brief Transpose this rotation matrix in place.
     * @return Reference to the transposed rotation matrix.
     */
    void transpose();

    /**
     * @brief Get the transpose of this rotation matrix.
     * @return Transposed rotation matrix.
     */
    Rotation3<T> transposed() const;

    /**
     * @brief Multiply this rotation matrix with another.
     * @param other Other rotation matrix.
     * @return Result of the multiplication.
     */
    Rotation3<T> operator*(const Rotation3<T>& other) const;

    /**
     * @brief Multiply this rotation matrix with another in place.
     * @param other Other rotation matrix.
     * @return Reference to the modified rotation matrix.
     */
    Rotation3<T>& operator*=(const Rotation3<T>& other);

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

template <typename T>
Point3<T> operator*(const Rotation3<T>& left, const Point3<T>& p);

template <typename T>
Vector3<T> operator*(const Rotation3<T>& left, const Vector3<T>& v);

V_MATH_NS_END
