#pragma once

#include "Point3d.h"
#include "Vector3d.h"

VI_GE_NS_BEGIN
class VI_GE_API Line
{
public:
    Line(const Point3d &origin, const Vector3d &direction);

public:
    bool intersectWith(const Line &line, Point3d &intersectionPt, double tol) const;

public:
    Point3d origin;
    Vector3d direction;
};
VI_GE_NS_END