#include <gtest/gtest.h>

#include <vine/math/Math.hpp>
#include <vine/math/Rect3.hpp>
#include <vine/math/Point3.hpp>

using namespace vine::math;

TEST(Rect, expandBy) {
    Rect3d r1;
    Rect3d r2;

    r1.expandBy(Point3d(1, 10, 10));
    r1.expandBy(Point3d(-1, 15, -6));

    r2.x = -6;
    r2.y = -6;
    r2.z = -6;
    r2.l = 10;
    r2.w = 10;
    r2.h = 10;

    r1.expandBy(r2);

    int a = 1;
}
