#pragma once

#include "line3d.h"

VI_GE_NS_BEGIN
class VI_GE_API Plane
{
public:
    Plane(const Point3d& origin, const Vector3d& normal);

public:
    bool intersectWith(const Line3d &line, Point3d &intersectionPt, double tol) const;

public:
    Point3d origin;
    Vector3d normal;
};
VI_GE_NS_END