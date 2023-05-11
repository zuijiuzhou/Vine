#include <ge/Line3d.h>

namespace etd
{
    namespace ge
    {
        Line3d::Line3d(Point3d origin, Vector3d direction)
            : m_origin(origin)
            , m_direction(direction)
        {
            
        }

        bool Line3d::intersectWith(const Line3d& line, Point3d& intersectionPt) const{
            if(line.m_origin == m_origin){
                intersectionPt = m_origin;
                return true;
            }

            return false;
        }
    }
}