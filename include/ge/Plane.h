#pragma once

#include "line3d.h"

VINE_GE_NS_BEGIN
class VINE_GE_API Plane
{
public:
    Plane(Point3d origin, Vector3d normal);

public:
    bool intersectWith(const Line3d &line, Point3d &intersectionPt, double tol) const;

public:
    Point3d origin;
    Vector3d normal;
};
VINE_GE_NS_END