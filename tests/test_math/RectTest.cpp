#include <gtest/gtest.h>

#include <vine/math/Math.hpp>
#include <vine/math/Point2.hpp>
#include <vine/math/Rect3.hpp>
#include <vine/math/Rect2.hpp>
#include <vine/math/Point3.hpp>

using namespace vine::math;

TEST(Rect2, corners)
{
    Rect2d r(2.0, 3.0, -5.0, 7.0);

    auto bl = r.bottomLeft();
    auto tl = r.topLeft();
    auto tr = r.topRight();

    EXPECT_DOUBLE_EQ(bl.x, -3.0);
    EXPECT_DOUBLE_EQ(bl.y, 3.0);
    EXPECT_DOUBLE_EQ(tl.x, -3.0);
    EXPECT_DOUBLE_EQ(tl.y, 10.0);
    EXPECT_DOUBLE_EQ(tr.x, 2.0);
    EXPECT_DOUBLE_EQ(tr.y, 10.0);
}

TEST(Rect2, intersectWith)
{
    Rect2d a(0.0, 0.0, 10.0, 10.0);
    Rect2d b(5.0, -2.0, 10.0, 5.0);
    auto   i = a.intersectWith(b);

    auto bl = i.bottomLeft();
    auto tr = i.topRight();

    EXPECT_DOUBLE_EQ(bl.x, 5.0);
    EXPECT_DOUBLE_EQ(bl.y, 0.0);
    EXPECT_DOUBLE_EQ(tr.x, 10.0);
    EXPECT_DOUBLE_EQ(tr.y, 3.0);

    Rect2d c(20.0, 20.0, 2.0, 2.0);
    auto   e = a.intersectWith(c);
    EXPECT_TRUE(e.isZero());
}

TEST(Rect3, expandBy)
{
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

    auto lb = r1.lowerBound();
    auto ub = r1.upperBound();

    EXPECT_DOUBLE_EQ(lb.x, -6.0);
    EXPECT_DOUBLE_EQ(lb.y, -6.0);
    EXPECT_DOUBLE_EQ(lb.z, -6.0);
    EXPECT_DOUBLE_EQ(ub.x, 4.0);
    EXPECT_DOUBLE_EQ(ub.y, 15.0);
    EXPECT_DOUBLE_EQ(ub.z, 10.0);
}

TEST(Rect3, intersectWith)
{
    Rect3d a(0.0, 0.0, 0.0, 10.0, 10.0, 10.0);
    Rect3d b(5.0, -2.0, 3.0, 10.0, 5.0, 10.0);
    auto i = a.intersectWith(b);

    auto lb = i.lowerBound();
    auto ub = i.upperBound();

    EXPECT_DOUBLE_EQ(lb.x, 5.0);
    EXPECT_DOUBLE_EQ(lb.y, 0.0);
    EXPECT_DOUBLE_EQ(lb.z, 3.0);
    EXPECT_DOUBLE_EQ(ub.x, 10.0);
    EXPECT_DOUBLE_EQ(ub.y, 3.0);
    EXPECT_DOUBLE_EQ(ub.z, 10.0);

    Rect3d c(20.0, 20.0, 20.0, 2.0, 2.0, 2.0);
    auto   e = a.intersectWith(c);
    EXPECT_TRUE(e.isZero());
}
