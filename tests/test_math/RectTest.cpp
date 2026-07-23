#include <gtest/gtest.h>

#include <vine/math/Math.hpp>
#include <vine/math/Point2.hpp>
#include <vine/math/Rect2.hpp>
#include <vine/math/Rect3.hpp>
#include <vine/math/Point3.hpp>

using namespace vine::math;

TEST(Rect2, minMax)
{
    // Construct with (xmin, ymin, xmax, ymax).
    Rect2d r(-3.0, 3.0, 2.0, 10.0);

    auto mn = r.min();
    auto mx = r.max();

    EXPECT_DOUBLE_EQ(mn.x, -3.0);
    EXPECT_DOUBLE_EQ(mn.y,  3.0);
    EXPECT_DOUBLE_EQ(mx.x,  2.0);
    EXPECT_DOUBLE_EQ(mx.y, 10.0);

    EXPECT_DOUBLE_EQ(r.width(),  5.0);
    EXPECT_DOUBLE_EQ(r.height(), 7.0);

    auto c = r.center();
    EXPECT_DOUBLE_EQ(c.x, -0.5);
    EXPECT_DOUBLE_EQ(c.y,  6.5);
}

TEST(Rect2, intersectWith)
{
    Rect2d a(0.0, 0.0, 10.0, 10.0);
    Rect2d b(5.0, -2.0, 15.0, 3.0);
    Rect2d i;
    bool   ok = a.intersectWith(b, i);

    EXPECT_TRUE(ok);
    auto mn = i.min();
    auto mx = i.max();
    EXPECT_DOUBLE_EQ(mn.x, 5.0);
    EXPECT_DOUBLE_EQ(mn.y, 0.0);
    EXPECT_DOUBLE_EQ(mx.x, 10.0);
    EXPECT_DOUBLE_EQ(mx.y, 3.0);

    // disjoint → returns false, out = gap rect (normalized)
    Rect2d c(20.0, 20.0, 22.0, 22.0);
    ok = a.intersectWith(c, i);
    EXPECT_FALSE(ok);
    EXPECT_FALSE(i.isNeg());
    EXPECT_DOUBLE_EQ(i.min().x, 10.0);
    EXPECT_DOUBLE_EQ(i.max().x, 20.0);
}

TEST(Rect3, expandBy)
{
    Rect3d r1;

    r1.expandBy(Point3d(1, 10, 10));
    r1.expandBy(Point3d(-1, 15, -6));

    Rect3d r2(-6.0, -6.0, -6.0, 4.0, 4.0, 4.0);
    r1.expandBy(r2);

    auto mn = r1.min();
    auto mx = r1.max();

    EXPECT_DOUBLE_EQ(mn.x, -6.0);
    EXPECT_DOUBLE_EQ(mn.y, -6.0);
    EXPECT_DOUBLE_EQ(mn.z, -6.0);
    EXPECT_DOUBLE_EQ(mx.x,  4.0);
    EXPECT_DOUBLE_EQ(mx.y, 15.0);
    EXPECT_DOUBLE_EQ(mx.z, 10.0);
}

TEST(Rect3, intersectWith)
{
    Rect3d a(0.0, 0.0, 0.0, 10.0, 10.0, 10.0);
    Rect3d b(5.0, -2.0, 3.0, 15.0, 3.0, 13.0);
    Rect3d i;
    bool   ok = a.intersectWith(b, i);

    EXPECT_TRUE(ok);
    auto mn = i.min();
    auto mx = i.max();
    EXPECT_DOUBLE_EQ(mn.x, 5.0);
    EXPECT_DOUBLE_EQ(mn.y, 0.0);
    EXPECT_DOUBLE_EQ(mn.z, 3.0);
    EXPECT_DOUBLE_EQ(mx.x, 10.0);
    EXPECT_DOUBLE_EQ(mx.y, 3.0);
    EXPECT_DOUBLE_EQ(mx.z, 10.0);

    // disjoint → returns false, out = gap box (normalized)
    Rect3d c(20.0, 20.0, 20.0, 22.0, 22.0, 22.0);
    ok = a.intersectWith(c, i);
    EXPECT_FALSE(ok);
    EXPECT_FALSE(i.isNeg());
    EXPECT_DOUBLE_EQ(i.min().x, 10.0);
    EXPECT_DOUBLE_EQ(i.max().x, 20.0);
}
