#pragma once

#include "Point3d.h"
#include "Vector3d.h"

VINE_GE_NS_BEGIN
class VINE_GE_API Line3d
{
public:
    Line3d(Point3d origin, Vector3d direction);

public:
    bool intersectWith(const Line3d &line, Point3d &intersectionPt, double tol) const;

public:
    Point3d origin;
    Vector3d direction;
};
VINE_GE_NS_END