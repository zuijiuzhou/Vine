#pragma once

#include "math_global.hpp"

#include <cstddef>

#include "Math.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

V_MATH_NS_BEGIN

/**
 * @brief Quaternion for 3D rotations, stored as (x, y, z, w) = (v, s).
 *
 * Convention: identity quaternion is (0, 0, 0, 1). Rotation of a vector
 * v is computed as q * v * q⁻¹ where q is a unit quaternion.
 *
 * Components:
 * - x, y, z: imaginary (vector) part.
 * - w: real (scalar) part.
 *
 * @tparam T Only accepts float and double.
 */
template <FP T>
class Quaternion3 {
  public:
    using value_type = T;

  public:
    /**
     * @brief Construct a zero quaternion.
     */
    constexpr Quaternion3()
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
    constexpr Quaternion3(T x, T y, T z, T w)
      : x(x)
      , y(y)
      , z(z)
      , w(w)
    {}

    /**
     * @brief Construct a unit quaternion from axis-angle rotation.
     *
     * q = (sin(θ/2)·axis_x, sin(θ/2)·axis_y, sin(θ/2)·axis_z, cos(θ/2)).
     *
     * @param angle Rotation angle θ in radians.
     * @param axis  Rotation axis (normalized internally, any non-zero vector is safe).
     */
    Quaternion3(T angle, const Vector3<T>& axis)
    { makeRotate(angle, axis); }

    /**
     * @brief Construct a unit quaternion that rotates from one direction to another.
     *
     * Computes the shortest-arc rotation between two unit vectors.
     *
     * @param from Source direction (normalized internally, any non-zero vector is safe).
     * @param to   Target direction (normalized internally, any non-zero vector is safe).
     */
    Quaternion3(const Vector3<T>& from, const Vector3<T>& to)
    { makeRotate(from, to); }

  public:
    /**
     * @brief Convert quaternion to 4D vector.
     * @return Vector containing x/y/z/w.
     */
    [[nodiscard]]
    constexpr Vector4<T> toVector() const
    { return Vector4<T>(x, y, z, w); }

    /**
     * @brief View quaternion as a 4D vector without copying.
     * @return Const reference to same memory as vector.
     */
    [[nodiscard]]
    constexpr const Vector4<T>& asVector() const
    {
        static_assert(sizeof(Quaternion3<T>) == sizeof(Vector4<T>));
        static_assert(std::is_standard_layout_v<Quaternion3<T>>);
        static_assert(std::is_standard_layout_v<Vector4<T>>);

        return reinterpret_cast<const Vector4<T>&>(*this);
    }

    /**
     * @brief Check if this quaternion is an identity rotation.
     * @param eps Tolerance used for comparison.
     */
    [[nodiscard]]
    constexpr bool isIdentity(T eps = EPS<T>()) const
    { return math::isZero(x, eps) && math::isZero(y, eps) && math::isZero(z, eps) && math::isEqual(w, T(1), eps); }

    /**
     * @brief Compute quaternion length.
     * @return Euclidean length.
     */
    [[nodiscard]]
    constexpr T length() const
    { return safeLength(x, y, z, w); }

    /**
     * @brief Compute squared quaternion length.
     * @return Squared length.
     */
    [[nodiscard]]
    constexpr T length2() const
    { return safeLengthSquared(x, y, z, w); }

    /**
     * @brief Compute the conjugate: q* = (-x, -y, -z, w).
     *
     * For a unit quaternion, the conjugate equals the inverse.
     *
     * @return Conjugated quaternion.
     */
    [[nodiscard]]
    constexpr Quaternion3<T> conj() const
    { return Quaternion3<T>(-x, -y, -z, w); }

    /**
     * @brief Invert this quaternion in place: q⁻¹ = conj(q) / |q|².
     *
     * For a unit quaternion, this is equivalent to conjugation.
     *
     * @note For a zero-length quaternion (|q| = 0), the quaternion is left unchanged.
     */
    constexpr void invert()
    {
        // q⁻¹ = conj(q) / |q|²
        const auto len2 = length2();
        if (len2 == T(0)) {
            return;
        }
        const auto rcp = T(1) / len2;

        x = -x * rcp;
        y = -y * rcp;
        z = -z * rcp;
        w =  w * rcp;
    }

    /**
     * @brief Return an inverted copy without modifying the original quaternion.
     *
     * For a zero-length quaternion, returns the identity (0, 0, 0, 1).
     *
     * @return Inverted quaternion.
     */
    [[nodiscard]]
    constexpr Quaternion3<T> inverted() const
    {
        const auto len2 = length2();
        if (len2 == T(0)) {
            return Quaternion3<T>(T(0), T(0), T(0), T(1));
        }
        return conj() / len2;
    }

    /**
     * @brief Set this quaternion from axis-angle: q = (sin(θ/2)·axis, cos(θ/2)).
     * @param angle Rotation angle θ in radians.
     * @param axis  Rotation axis (normalized internally, any non-zero vector is safe).
     */
    void makeRotate(T angle, const Vector3<T>& axis);
    /**
     * @brief Set this quaternion to the shortest-arc rotation from one direction to another.
     * @param from Source direction (normalized internally, any non-zero vector is safe).
     * @param to   Target direction (normalized internally, any non-zero vector is safe).
     */
    void makeRotate(const Vector3<T>& from, const Vector3<T>& to);

    /**
     * @brief Extract axis-angle representation from this quaternion.
     *
     * For the identity quaternion (0,0,0,1), returns angle = 0 and axis = (1,0,0).
     *
     * @param o_angle Output rotation angle in radians, in [0, π].
     * @param o_axis  Output rotation axis, normalized.
     */
    void getRotate(T& o_angle, Vector3<T>& o_axis) const;

    // Vector3<T> toEuler() const;
    // void       fromEuler(const Vector3<T>& euler);

    /**
     * @brief Spherical linear interpolation (slerp) between two unit quaternions.
     *
     * Computes the shortest path on the 4D unit sphere:
     * slerp(q₀, q₁, t) = q₀ * (q₀⁻¹ * q₁)^t.
     *
     * @param from Start quaternion (should be unit).
     * @param to   End quaternion (should be unit).
     * @param t    Interpolation factor in [0, 1].
     * @return Interpolated unit quaternion.
     */
    static Quaternion3<T> slerp(const Quaternion3<T>& from, const Quaternion3<T>& to, T t);

  public:
    /**
     * @brief Equality operator.
     * @param right Right-hand quaternion.
     * @return True when components are equal.
     */
    [[nodiscard]]
    constexpr bool operator==(const Quaternion3& right) const
    { return x == right.x && y == right.y && z == right.z && w == right.w; }

    /**
     * @brief Inequality operator.
     * @param right Right-hand quaternion.
     * @return True when any component differs.
     */
    [[nodiscard]]
    constexpr bool operator!=(const Quaternion3& right) const
    { return !(*this == right); }

    /**
     * @brief Multiply quaternion by scalar: q' = (x·s, y·s, z·s, w·s).
     *
     * @note Scalar multiplication does not preserve unit length; the result
     *       generally does not represent a valid rotation. Used internally
     *       for normalization and interpolation.
     *
     * @param right Scalar multiplier.
     * @return Scaled quaternion.
     */
    constexpr Quaternion3<T> operator*(T right) const
    {
        Quaternion3<T> q;
        q.x = x * right;
        q.y = y * right;
        q.z = z * right;
        q.w = w * right;
        return q;
    }

    /**
     * @brief Multiply quaternion by scalar in place: q = (x·s, y·s, z·s, w·s).
     *
     * @note See operator*(T) for caveats about unit length.
     *
     * @param right Scalar multiplier.
     * @return Reference to this quaternion.
     */
    constexpr Quaternion3<T>& operator*=(T right)
    {
        x *= right;
        y *= right;
        z *= right;
        w *= right;
        return *this;
    }

    /**
     * @brief Divide quaternion by scalar: q' = (x/s, y/s, z/s, w/s).
     *
     * @note Scalar division does not preserve unit length and has no direct
     *       rotation meaning. Used internally (e.g. q / |q|² in inversion,
     *       normalization).
     *
     * @param right Scalar divisor.
     * @return Scaled quaternion.
     */
    [[nodiscard]]
    constexpr Quaternion3<T> operator/(T right) const
    {
        auto           rcp = T(1) / right;
        Quaternion3<T> q;
        q.x = x * rcp;
        q.y = y * rcp;
        q.z = z * rcp;
        q.w = w * rcp;
        return q;
    }

    /**
     * @brief Divide quaternion by scalar in place: q = (x/s, y/s, z/s, w/s).
     *
     * @note See operator/(T) for caveats about unit length.
     *
     * @param right Scalar divisor.
     * @return Reference to this quaternion.
     */
    constexpr Quaternion3<T>& operator/=(T right)
    {
        auto rcp = T(1) / right;
        x *= rcp;
        y *= rcp;
        z *= rcp;
        w *= rcp;
        return *this;
    }

    /**
     * @brief Quaternion addition: component-wise sum.
     *
     * @note Addition has no direct geometric (rotation) meaning — the result
     *       is generally not a unit quaternion. It exists primarily as an
     *       algebraic utility (e.g. for averaging with subsequent normalization).
     *
     * @param right Right-hand quaternion.
     * @return Sum quaternion.
     */
    [[nodiscard]]
    constexpr Quaternion3<T> operator+(const Quaternion3& right) const
    {
        Quaternion3<T> q;
        q.x = x + right.x;
        q.y = y + right.y;
        q.z = z + right.z;
        q.w = w + right.w;
        return q;
    }

    /**
     * @brief Quaternion addition in place: component-wise sum.
     *
     * @note See operator+ for caveats about geometric meaning.
     *
     * @param right Right-hand quaternion.
     * @return Reference to this quaternion.
     */
    constexpr Quaternion3<T>& operator+=(const Quaternion3& right)
    {
        x += right.x;
        y += right.y;
        z += right.z;
        w += right.w;
        return *this;
    }

    /**
     * @brief Quaternion subtraction: component-wise difference.
     *
     * @note Like addition, subtraction has no direct rotation meaning and
     *       exists as an algebraic utility.
     *
     * @param right Right-hand quaternion.
     * @return Difference quaternion.
     */
    [[nodiscard]]
    constexpr Quaternion3<T> operator-(const Quaternion3& right) const
    {
        Quaternion3<T> q;
        q.x = x - right.x;
        q.y = y - right.y;
        q.z = z - right.z;
        q.w = w - right.w;
        return q;
    }

    /**
     * @brief Quaternion subtraction in place: component-wise difference.
     *
     * @note See operator- for caveats about geometric meaning.
     *
     * @param right Right-hand quaternion.
     * @return Reference to this quaternion.
     */
    constexpr Quaternion3<T>& operator-=(const Quaternion3& right)
    {
        x -= right.x;
        y -= right.y;
        z -= right.z;
        w -= right.w;
        return *this;
    }

    /**
     * @brief Quaternion multiplication: q₁ * q₂.
     *
     * Represents rotation composition — applies q₂ first, then q₁:
     * rotate(v) = q₁ * (q₂ * v * q₂⁻¹) * q₁⁻¹ = (q₁q₂) * v * (q₁q₂)⁻¹.
     *
     * @param right Right-hand quaternion (applied first).
     * @return Product quaternion.
     */
    [[nodiscard]]
    Quaternion3<T> operator*(const Quaternion3& right) const;
    /**
     * @brief Quaternion multiplication in place: q = q * r.
     *
     * Represents rotation composition — applies r first, then the original q.
     *
     * @param right Right-hand quaternion (applied first).
     * @return Reference to this quaternion.
     */
    Quaternion3<T>& operator*=(const Quaternion3& right);

    /**
     * @brief Quaternion division: q₁ / q₂ = q₁ * q₂⁻¹.
     *
     * Represents the "difference" rotation from q₂ to q₁:
     * if q₁ = q_diff * q₂, then q_diff = q₁ / q₂.
     *
     * @param right Divisor quaternion.
     * @return Quotient quaternion.
     */
    [[nodiscard]]
    Quaternion3<T> operator/(const Quaternion3& right) const;
    /**
     * @brief Quaternion division assignment: q /= r  →  q = q * r⁻¹.
     *
     * Replaces q with the "difference" rotation from r to the original q:
     * after this operation, q * r equals the original q (up to the same
     * rotation, since q and -q are equivalent).
     *
     * @param right Divisor quaternion.
     * @return Reference to this quaternion.
     */
    Quaternion3<T>& operator/=(const Quaternion3& right);

    /**
     * @brief Unary negation: -q = (-x, -y, -z, -w).
     *
     * For unit quaternions, q and -q represent the **same rotation**
     * (quaternions double-cover SO(3)). This is because the rotation
     * formula q·v·q⁻¹ is invariant under q → -q.
     *
     * For non-unit quaternions, this is simply component-wise negation.
     *
     * @return Negated quaternion.
     */
    [[nodiscard]]
    constexpr Quaternion3<T> operator-() const
    { return Quaternion3<T>(-x, -y, -z, -w); }

    /**
     * @brief Access component by index.
     * @param i Component index in [0, 3].
     * @return Mutable component reference.
     */
    [[nodiscard]]
    constexpr T& operator[](size_t i)
    { return data[i]; }

    /**
     * @brief Access component by index.
     * @param i Component index in [0, 3].
     * @return Component value.
     */
    [[nodiscard]]
    constexpr T operator[](size_t i) const
    { return data[i]; }

  public:
    union
    {
        struct {
            T x, y, z, w;
        };

        T data[4];
    };
};

using Quatf = Quaternion3<float>;
using Quatd = Quaternion3<double>;

template <typename T>
Vector3<T> operator*(const Quaternion3<T>& left, const Vector3<T>& right);

V_MATH_NS_END
