#include <gtest/gtest.h>

#include <vine/ge/Math.h>
#include <vine/ge/Types.h>

using namespace vine::ge;



TEST(Vector, cross) {
    Vec3d v1(1, 9, 10);
    Vec3d v2(-1, 1, 0);
    Vec3d v3(-1, -1, 0);
    Vec3i ref1(0, 0, 1);
    Vec3i ref2(0, 0, -1);

    //auto n1 = normalize(v1);

    auto v12 = cross(v1, v2);

    auto d1 = v1 * v3;

    int x = 0;
    // double theta = v1.angleTo(v2, ref1);
    // double theta2 = v1.angleTo(v3, ref1);
    // double theta3 = v1.angleTo(v2, ref2);
    // double theta4 = v1.angleTo(v3, ref2);
    // ASSERT_TRUE(theta < PI);
}
