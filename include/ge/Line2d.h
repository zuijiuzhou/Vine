#pragma once

#include "ge_export.h"

#include "Point2d.h"
#include "Vector2d.h"

ETD_GE_NS_BEGIN
class ETD_GE_API Line2d
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
ETD_GE_NS_END