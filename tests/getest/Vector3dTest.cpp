#include <gtest/gtest.h>

#include <ge/Vector3d.h>
#include <ge/Ge.h>

using namespace vine::ge;



TEST(Vector3d, angleTo){
    Vector3d v1(1,0,0);
    Vector3d v2(-1,1, 0);
    Vector3d v3(-1,-1, 0);
    Vector3d ref1(0,0,1);
    Vector3d ref2(0,0,-1);

    double theta = v1.angleTo(v2, ref1);
    double theta2 = v1.angleTo(v3, ref1);
    double theta3 = v1.angleTo(v2, ref2);
    double theta4 = v1.angleTo(v3, ref2);
    ASSERT_TRUE(theta < PI);
}
