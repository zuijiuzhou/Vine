#include <gtest/gtest.h>

#include <ge/Matrix4x4.h>

using namespace vine::ge;



TEST(Matrix4x4, setToIdentity){
    Matrix4x4 m;
    m(1,3)=6;
    ASSERT_TRUE(m(0,0)==m(1,1)==m(2,2)==m(3,3) == 1. && m(0,1) == m(2,3) == m(1,2) == 0.);
}

TEST(Matrix4x4, transpose){
    Matrix4x4 m;
    m(1,3)=6;
    m.transpose();
    ASSERT_TRUE(m(3,1) == 6);
}