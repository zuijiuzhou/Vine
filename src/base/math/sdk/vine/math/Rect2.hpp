#pragma once

#include "math_global.hpp"

#include <cstdint>

#include "Point2.hpp"
#include "Vector2.hpp"

VI_MATH_NS_BEGIN

/**
 * @brief A class representing a rectangle in 2D space, defined by its top-left corner and size.
 *        The rectangle is axis-aligned, meaning its edges are parallel to the coordinate axes.
 * @tparam T Only accepts float double and integers(not boolean)
 */
template <typename T>
class Rect2 {
  public:
    using value_type = T;

  public:
    constexpr Rect2()
      : x(T())
      , y(T())
      , w(T())
      , h(T())
    {}

    constexpr Rect2(const Point2<T>& corner, const Vector2<T>& size)
      : x(corner.x)
      , y(corner.y)
      , w(size.x)
      , h(size.y)
    {}

    constexpr Rect2(T xx, T yy, T ww, T hh)
      : x(xx)
      , y(yy)
      , w(ww)
      , h(hh)
    {}

  public:
    [[nodiscard]]
    constexpr T top() const
    {
        return std::max<T>(y, y + h);
    }

    [[nodiscard]]
    constexpr T bottom() const
    {
        return std::min<T>(y, y + h);
    }

    [[nodiscard]]
    constexpr T left() const
    {
        return std::min<T>(x, x + w);
    }

    [[nodiscard]]
    constexpr T right() const
    {
        return std::max<T>(x, x + w);
    }

    // the min corner
    [[nodiscard]]
    constexpr Point2<T> bottomLeft() const
    {
        return Point2<T>(std::min<T>(x, x + w), std::min<T>(y, y + h));
    }

    [[nodiscard]]
    constexpr Point2<T> bottomRight() const
    {
        return Point2<T>(std::max<T>(x, x + w), std::min<T>(y, y + h));
    }

    // the max corner
    [[nodiscard]]
    constexpr Point2<T> topLeft() const
    {
        return Point2<T>(std::min<T>(x, x + w), std::min<T>(y, y + h));
    }

    [[nodiscard]]
    constexpr Point2<T> topRight() const
    {
        return Point2<T>(std::max<T>(x, x + w), std::max<T>(y, y + h));
    }

    [[nodiscard]]
    constexpr T width() const
    {
        return w;
    }

    [[nodiscard]]
    constexpr T height() const
    {
        return h;
    }

    [[nodiscard]]
    constexpr Vector2<T> size() const
    {
        return Vector2<T>(w, h);
    }

    [[nodiscard]]
    bool contains(T x, T y) const;
    [[nodiscard]]
    bool contains(const Point2<T>& pt) const;

    void expandBy(const Vector2<T>& pt);
    void expandBy(const Rect2<T>& rect);

    [[nodiscard]]
    constexpr bool isZero() const
    {
        return w == T() && h == T();
    }

    [[nodiscard]]
    constexpr bool isZero(T eps) const requires(Real<T>)
    {
        return math::isZero<T>(w, eps) && math::isZero<T>(h, eps);
    }

    [[nodiscard]]
    constexpr bool operator==(const Rect2<T>& right) const
    {
        return x == right.x && y == right.y && w == right.w && h == right.h;
    }

    [[nodiscard]]
    constexpr bool operator!=(const Rect2<T>& right) const
    {
        return !(*this == right);
    }

  public:
    union
    {
        struct {
            T x, y, w, h;
        };

        T data[4];
    };
};

using Rect2i = Rect2<int32_t>;
using Rect2f = Rect2<float>;
using Rect2d = Rect2<double>;

VI_MATH_NS_END
