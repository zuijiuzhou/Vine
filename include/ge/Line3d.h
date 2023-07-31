#pragma once

#include "Point3d.h"
#include "Vector3d.h"

ETD_GE_NS_BEGIN
class ETD_GE_API Line3d
{
public:
    Line3d(Point3d origin, Vector3d direction);

public:
    bool intersectWith(const Line3d &line, Point3d &intersectionPt, double tol) const;

public:
    Point3d origin;
    Vector3d direction;
};
ETD_GE_NS_END