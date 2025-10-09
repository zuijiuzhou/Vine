#include <gtest/gtest.h>

#include <vine/ge/Math.hpp>

using namespace vine::ge;



TEST(Vector, cross) {
    Vec3f v1(1, 9, 10);
    Vec3f v2(-1, 1, 0);
    Vec3f v3(-1, -1, 0);
    Vec3i ref1(0, 0, 1);
    Vec3i ref2(0, 0, -1);

    Vector2<bool> vb1;
}
