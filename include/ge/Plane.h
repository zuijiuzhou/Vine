#pragma once

#include "line3d.h"

namespace etd
{
    namespace ge
    {
        class ETD_GE_EXPORT Plane
        {
        public:
            Plane(Point3d origin, Vector3d normal);

        public:
            bool intersectWith(const Line3d& line, Point3d& intersectionPt, double tol) const;

        public:
            Point3d origin;
            Vector3d normal;
        };
    }
}