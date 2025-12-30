#include <gtest/gtest.h>

#include <vine/math/Math.hpp>
#include <vine/math/Vector4.hpp>

using namespace vine::math;

TEST(Vector, cross)
{
    // Vec3f v1(1, 9, 10);
    // Vec3f v2(-1, 1, 0);
    // Vec3f v3(-1, -1, 0);
    // Vec3i ref1(0, 0, 1);
    // Vec3i ref2(0, 0, -1);

    // Vector2<bool> vb1;

    Vec4i v1(1, 2, 3, 4);
    Vec4i v2(10, 11, 12, 13);

    auto v3 = v1 + v2;
    auto v4 = v1 - v2;
    auto v6 = v1 * 10;

    Vec4b b1(true, false, true, false);
    Vec4b b2(false, false, true, true);

    auto b3 = b2 / true;
    auto b4 = b1 * true;
    auto b5 = b1 + b2;
    auto b6 = b1 - b2;

}
