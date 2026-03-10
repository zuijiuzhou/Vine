#pragma once

#include "math_global.hpp"

#include <cstddef>

#include "Math.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

VI_MATH_NS_BEGIN

/**
 * @brief A class representing a quaternion for 3D rotations
 * @tparam T Only accepts float and double
 */
template <FP T>
class Quaternion {
  public:
    using value_type = T;

  public:
        /**
         * @brief Construct a zero quaternion.
         */
    constexpr Quaternion()
      : x(T())
      , y(T())
      , z(T())
      , w(T())
    {}

        /**
         * @brief Construct quaternion from components.
         * @param x X component.
         * @param y Y component.
         * @param z Z component.
         * @param w W component.
         */
    constexpr Quaternion(T x, T y, T z, T w)
      : x(x)
      , y(y)
      , z(z)
      , w(w)
    {}

        /**
         * @brief Construct quaternion from axis-angle rotation.
         * @param angle Rotation angle in radians.
         * @param axis Rotation axis.
         */
    Quaternion(T angle, const Vector3<T>& axis)
    {
        makeRotate(angle, axis);
    }

        /**
         * @brief Construct quaternion rotating one direction to another.
         * @param from Source direction.
         * @param to Target direction.
         */
    Quaternion(const Vector3<T>& from, const Vector3<T>& to)
    {
        makeRotate(from, to);
    }

  public:
        /**
         * @brief Convert quaternion to 4D vector.
         * @return Vector containing x/y/z/w.
         */
    [[nodiscard]]
    constexpr Vector4<T> toVector() const
    {
        return Vector4<T>(x, y, z, w);
    }

    /**
     * @brief View quaternion as a 4D vector without copying.
     * @return Const reference to same memory as vector.
     */
    [[nodiscard]]
    constexpr const Vector4<T>& asVector() const
    {
        static_assert(sizeof(Quaternion<T>) == sizeof(Vector4<T>));
        static_assert(std::is_standard_layout_v<Quaternion<T>>);
        static_assert(std::is_standard_layout_v<Vector4<T>>);

        return reinterpret_cast<const Vector4<T>&>(*this);
    }

    /**
     * @brief Compute quaternion length.
     * @return Euclidean length.
     */
    [[nodiscard]]
    constexpr T lenght() const
    {
        return safeLength(x, y, z, w);
    }

    /**
     * @brief Compute squared quaternion length.
     * @return Squared length.
     */
    [[nodiscard]]
    constexpr T lenght2() const
    {
        return safeLengthSquared(x, y, z, w);
    }

    /**
     * @brief Compute conjugate quaternion.
     * @return Conjugated quaternion.
     */
    [[nodiscard]]
    constexpr Quaternion<T> conj() const
    {
        return Quaternion<T>(-x, -y, -z, w);
    }

    /**
     * @brief Invert this quaternion in-place.
     */
    constexpr void invert()
    {
        // q-1 = conj / len2
        auto len2 = lenght2();
        auto rcp  = T(1) / len2;

        x = -x * rcp;
        y = -y * rcp;
        z = -z * rcp;
        w = w * rcp;
    }

    /**
     * @brief Get inverse quaternion without modifying this instance.
     * @return Inverted quaternion.
     */
    [[nodiscard]]
    constexpr Quaternion<T> inverted() const
    {
        return conj() / lenght2();
    }

    /**
     * @brief Set quaternion from axis-angle rotation.
     * @param angle Rotation angle in radians.
     * @param axis Rotation axis.
     */
    void makeRotate(T angle, const Vector3<T>& axis);
    /**
     * @brief Set quaternion rotating one direction to another.
     * @param from Source direction.
     * @param to Target direction.
     */
    void makeRotate(const Vector3<T>& from, const Vector3<T>& to);

    /**
     * @brief Extract axis-angle rotation from quaternion.
     * @param o_angle Output rotation angle in radians.
     * @param o_axis Output rotation axis.
     */
    void getRotate(T& o_angle, Vector3<T>& o_axis) const;

    // Vector3<T> toEuler() const;
    // void       fromEuler(const Vector3<T>& euler);

        /**
         * @brief Spherical linear interpolation between two quaternions.
         * @param from Start quaternion.
         * @param to End quaternion.
         * @param t Interpolation factor in [0, 1].
         * @return Interpolated quaternion.
         */
    static Quaternion<T> slerp(const Quaternion<T>& from, const Quaternion<T>& to, T t);

  public:
        /**
         * @brief Equality operator.
         * @param right Right-hand quaternion.
         * @return True when components are equal.
         */
    [[nodiscard]]
    constexpr bool operator==(const Quaternion& right) const
    {
        return x == right.x && y == right.y && z == right.z && w == right.w;
    }

    /**
     * @brief Inequality operator.
     * @param right Right-hand quaternion.
     * @return True when any component differs.
     */
    [[nodiscard]]
    constexpr bool operator!=(const Quaternion& right) const
    {
        return !(*this == right);
    }

    /**
     * @brief Multiply quaternion by scalar.
     * @param right Scalar multiplier.
     * @return Scaled quaternion.
     */
    constexpr Quaternion<T> operator*(T right) const
    {
        Quaternion<T> q;
        q.x = x * right;
        q.y = y * right;
        q.z = z * right;
        q.w = w * right;
        return q;
    }

    /**
     * @brief Multiply quaternion by scalar in-place.
     * @param right Scalar multiplier.
     * @return Reference to this quaternion.
     */
    constexpr Quaternion<T>& operator*=(T right)
    {
        x *= right;
        y *= right;
        z *= right;
        w *= right;
        return *this;
    }

    /**
     * @brief Divide quaternion by scalar.
     * @param right Scalar divisor.
     * @return Scaled quaternion.
     */
    [[nodiscard]]
    constexpr Quaternion<T> operator/(T right) const
    {
        auto          rcp = T(1) / right;
        Quaternion<T> q;
        q.x = x * rcp;
        q.y = y * rcp;
        q.z = z * rcp;
        q.w = w * rcp;
        return q;
    }

    /**
     * @brief Divide quaternion by scalar in-place.
     * @param right Scalar divisor.
     * @return Reference to this quaternion.
     */
    constexpr Quaternion<T>& operator/=(T right)
    {
        auto rcp = T(1) / right;
        x *= rcp;
        y *= rcp;
        z *= rcp;
        w *= rcp;
        return *this;
    }

    /**
     * @brief Quaternion addition.
     * @param right Right-hand quaternion.
     * @return Sum quaternion.
     */
    [[nodiscard]]
    constexpr Quaternion<T> operator+(const Quaternion& right) const
    {
        Quaternion<T> q;
        q.x = x + right.x;
        q.y = y + right.y;
        q.z = z + right.z;
        q.w = w + right.w;
        return q;
    }

    /**
     * @brief Quaternion addition assignment.
     * @param right Right-hand quaternion.
     * @return Reference to this quaternion.
     */
    constexpr Quaternion<T>& operator+=(const Quaternion& right)
    {
        x += right.x;
        y += right.y;
        z += right.z;
        w += right.w;
        return *this;
    }

    /**
     * @brief Quaternion subtraction.
     * @param right Right-hand quaternion.
     * @return Difference quaternion.
     */
    [[nodiscard]]
    constexpr Quaternion<T> operator-(const Quaternion& right) const
    {
        Quaternion<T> q;
        q.x = x - right.x;
        q.y = y - right.y;
        q.z = z - right.z;
        q.w = w - right.w;
        return q;
    }

    /**
     * @brief Quaternion subtraction assignment.
     * @param right Right-hand quaternion.
     * @return Reference to this quaternion.
     */
    constexpr Quaternion<T>& operator-=(const Quaternion& right)
    {
        x -= right.x;
        y -= right.y;
        z -= right.z;
        w -= right.w;
        return *this;
    }

    /**
     * @brief Quaternion multiplication.
     * @param right Right-hand quaternion.
     * @return Product quaternion.
     */
    [[nodiscard]]
    Quaternion<T>  operator*(const Quaternion& right) const;
    /**
     * @brief Quaternion multiplication assignment.
     * @param right Right-hand quaternion.
     * @return Reference to this quaternion.
     */
    Quaternion<T>& operator*=(const Quaternion& right);

    /**
     * @brief Quaternion division.
     * @param right Right-hand quaternion.
     * @return Quotient quaternion.
     */
    [[nodiscard]]
    Quaternion<T>  operator/(const Quaternion& right) const;
    /**
     * @brief Quaternion division assignment.
     * @param right Right-hand quaternion.
     * @return Reference to this quaternion.
     */
    Quaternion<T>& operator/=(const Quaternion& right);

    /**
     * @brief Unary negation.
     * @return Quaternion with negated components.
     */
    [[nodiscard]]
    constexpr Quaternion<T> operator-() const
    {
        return Quaternion<T>(-x, -y, -z, -w);
    }

    /**
     * @brief Access component by index.
     * @param i Component index in [0, 3].
     * @return Mutable component reference.
     */
    [[nodiscard]]
    constexpr T& operator[](size_t i)
    {
        return data[i];
    }

    /**
     * @brief Access component by index.
     * @param i Component index in [0, 3].
     * @return Component value.
     */
    [[nodiscard]]
    constexpr T operator[](size_t i) const
    {
        return data[i];
    }

  public:
    union
    {
        struct {
            T x, y, z, w;
        };

        T data[4];
    };
};

using Quatf = Quaternion<float>;
using Quatd = Quaternion<double>;

template <typename T>
Vector3<T> operator*(const Quaternion<T>& left, const Vector3<T>& right);

VI_MATH_NS_END
