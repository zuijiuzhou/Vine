#pragma once

#include "etd_ge_export.h"

#include "Point3d.h"
#include "Vector3d.h"

namespace etd
{
    namespace ge
    {
        class ETD_GE_API Line3d
        {
        public:
            Line3d(Point3d origin, Vector3d direction);

        public:
            Point3d origin() const;
            Vector3d direction() const;

            bool intersectWith(const Line3d &line, Point3d &intersectionPt) const;

        public:
            Point3d m_origin;
            Vector3d m_direction;
        };
    }
}