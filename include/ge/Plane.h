#pragma once

#include "etd_ge_export.h"

#include "Point3d.h"
#include "Vector3d.h"

namespace etd{
    namespace ge{
        class ETD_GE_EXPORT Plane{
            public:
            Plane(Point3d origin, Vector3d normal);
            public:
            double origin() const;
            double normal() const;
            public:
            Point3d m_origin;
            Vector3d m_normal;
        };
    }
}