#pragma once

#include "line3d.h"

ETD_GE_NS_BEGIN
class ETD_GE_API Plane
{
public:
    Plane(Point3d origin, Vector3d normal);

public:
    bool intersectWith(const Line3d &line, Point3d &intersectionPt, double tol) const;

public:
    Point3d origin;
    Vector3d normal;
};
ETD_GE_NS_END