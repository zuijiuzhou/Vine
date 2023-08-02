#pragma once

#include "ge_global.h"

#include "Point2d.h"
#include "Vector2d.h"

VINE_GE_NS_BEGIN
class VINE_GE_API Line2d
{
public:
    Line2d(Point2d origin, Vector2d direction);

public:
    Point2d origin() const;
    Vector2d direction() const;

    bool intersectWith(const Line2d &line, Point2d &intersectionPt) const;

public:
    Point2d m_origin;
    Vector2d m_direction;
};
VINE_GE_NS_END