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

    /**
     * @brief Construct a rotation from raw elements in row-major order.
     *
     * Parameters follow the logical matrix layout:
     * | m00 m01 m02 |
     * | m10 m11 m12 |
     * | m20 m21 m22 |
     * Caller must ensure the matrix is orthonormal (det = 1).
     *
     * @param _m00 Row 0, col 0. @param _m01 Row 0, col 1. @param _m02 Row 0, col 2.
     * @param _m10 Row 1, col 0. @param _m11 Row 1, col 1. @param _m12 Row 1, col 2.
     * @param _m20 Row 2, col 0. @param _m21 Row 2, col 1. @param _m22 Row 2, col 2.
     */
    // clang-format off
    constexpr Rotation3(T _m00, T _m01, T _m02,
                        T _m10, T _m11, T _m12,
                        T _m20, T _m21, T _m22) noexcept

      : m00(_m00), m01(_m01), m02(_m02) // row0
      , m10(_m10), m11(_m11), m12(_m12) // row1
      , m20(_m20), m21(_m21), m22(_m22) // row2
    {}
    // clang-format on

    /**
     * @brief Construct a rotation from a 9-element array in column-major order.
     *
     * elements[0..8] match the internal data[] layout:
     * col0: elements[0..2], col1: elements[3..5], col2: elements[6..8].
     *
     * @warning The pointer is not validated. Passing nullptr or fewer than
     *          9 elements results in undefined behavior. Prefer the array-
     *          reference overload when the array size is known at compile time.
     *
     * @param elements Pointer to 9 T values in column-major layout.
     */
    explicit Rotation3(const T* elements) noexcept
      : m00(elements[0]), m10(elements[1]), m20(elements[2])
      , m01(elements[3]), m11(elements[4]), m21(elements[5])
      , m02(elements[6]), m12(elements[7]), m22(elements[8])
    {}

    /**
     * @brief Construct a rotation from a 9-element array in column-major order
     *        (compile-time size check).
     *
     * @param elements Array of 9 T values in column-major layout.
     */
    explicit Rotation3(const T (&elements)[9]) noexcept
      : Rotation3(static_cast<const T*>(elements))
    {}

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
