#include <gtest/gtest.h>

#include <vine/ge/Matrix4x4.h>

using namespace vine::ge;

TEST(Matrix4x4, setToIdentity) {
    Mat4d m;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (i == j) {
                ASSERT_TRUE(m(i, j) == 1.0);
            }
            else {
                ASSERT_TRUE(m(i, j) == 0.0);
            }
        }
    }
}

TEST(Matrix4x4, transpose) {
    Mat4d m;
    m(1, 3) = 6;
    m.transpose();
    ASSERT_TRUE(m(3, 1) == 6);
}