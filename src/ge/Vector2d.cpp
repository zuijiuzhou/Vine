#include "ge/Vector2d.h"

#include <cmath>

namespace etd
{
    namespace ge
    {
        Vector2d::Vector2d() : Vector2d(0., 0.)
        {
        }
        Vector2d::Vector2d(double xx, double yy)
            : x(xx), y(yy)
        {
        }

        double Vector2d::length() const
        {
            return sqrt(x * x + y * y);
        }
    } // namespace ge
}