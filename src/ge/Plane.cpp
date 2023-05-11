#include <ge/Plane.h>

namespace etd
{
    namespace ge
    {
        Plane::Plane(Point3d origin, Vector3d normal)
            : m_origin(origin)
            , m_normal(normal)
        {

        }
    }
}