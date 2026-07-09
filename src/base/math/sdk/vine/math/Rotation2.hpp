#pragma once

#include "math_global.hpp"

#include <cmath>

V_MATH_NS_BEGIN
template <typename T>
class Point2;
template <typename T>
class Vector2;

/**
 * @brief 2D rotation as a 2×2 column-major matrix.
 * @tparam T Only accepts float and double.
 *
 * Structurally mirrors Rotation3. Columns contain the rotated basis vectors:
 *   Column 0 = X axis after rotation = (cos θ, sin θ)
 *   Column 1 = Y axis after rotation = (-sin θ, cos θ)
 *
 * Conventions:
 * - Positive θ produces a counter-clockwise (CCW) rotation.
 * - transpose() / transposed() swap the off-diagonal, giving the inverse
 *   rotation (equivalent to rotating by -θ).
 */
template <typename T>
class Rotation2 {
  public:
    using value_type = T;

  public:
    /**
     * @brief Construct an identity rotation.
     */
    constexpr Rotation2() noexcept
      : data{ T(1), T(0), T(0), T(1) }
    {}

    /**
     * @brief Construct a rotation from an angle.
     * @param angle Rotation angle in radians (CCW).
     */
    explicit Rotation2(T angle) noexcept
    {
        const auto c = std::cos(angle);
        const auto s = std::sin(angle);
        m00 = c;  m10 = s;   // col 0 = X axis
        m01 = -s; m11 = c;   // col 1 = Y axis
    }

  public:
    /**
     * @brief Transpose this rotation matrix in place.
     * @return Reference to the transposed rotation matrix.
     */
    void transpose()
    { std::swap(m01, m10); }

    /**
     * @brief Get the transpose of this rotation matrix.
     * @return Transposed rotation matrix.
     */
    [[nodiscard]]
    Rotation2<T> transposed() const
    {
        Rotation2<T> result = *this;
        result.transpose();
        return result;
    }

    /**
     * @brief Multiply this rotation matrix with another.
     * @param other Other rotation matrix.
     * @return Result of the multiplication.
     */
    [[nodiscard]]
    Rotation2<T> operator*(const Rotation2<T>& other) const
    {
        // Column-major 2x2 multiplication: C = A * B
        Rotation2<T> result;
        result.vecs[0] = vecs[0] * other.vecs[0].x + vecs[1] * other.vecs[0].y;
        result.vecs[1] = vecs[0] * other.vecs[1].x + vecs[1] * other.vecs[1].y;
        return result;
    }

    /**
     * @brief Multiply this rotation matrix with another in place.
     * @param other Other rotation matrix.
     * @return Reference to the modified rotation matrix.
     */
    Rotation2<T>& operator*=(const Rotation2<T>& other)
    {
        const Vector2<T> a0 = vecs[0];
        const Vector2<T> a1 = vecs[1];

        vecs[0] = a0 * other.vecs[0].x + a1 * other.vecs[0].y;
        vecs[1] = a0 * other.vecs[1].x + a1 * other.vecs[1].y;

        return *this;
    }

  public:
    union
    {
        struct {
            T m00, m10; // col 0
            T m01, m11; // col 1
        };

        struct {
            Vector2<T> vec0;
            Vector2<T> vec1;
        };

        Vector2<T> vecs[2];

        T data[4];
    };
};

template <typename T>
Point2<T> operator*(const Rotation2<T>& left, const Point2<T>& p);

template <typename T>
Vector2<T> operator*(const Rotation2<T>& left, const Vector2<T>& v);

using Rotation2f = Rotation2<float>;
using Rotation2d = Rotation2<double>;

V_MATH_NS_END
