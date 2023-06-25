#include <ge/Plane.h>

namespace etd
{
    namespace ge
    {
        Plane::Plane(Point3d origin, Vector3d normal)
            : origin(origin)
            , normal(normal)
        {

        }

        bool Plane::intersectWith(const Line3d& line, Point3d& intersectionPt, double tol) const
        {
            return false;
        }
    }
}