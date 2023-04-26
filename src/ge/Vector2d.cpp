#include "ge/Vector2d.h"

#include<cmath>

namespace etd{
    namespace ge
    {
        double Vector2d::length() const{
            return sqrt(x*x +y*y);
        }
    } // namespace ge
}