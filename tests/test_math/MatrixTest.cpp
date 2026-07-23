#include <gtest/gtest.h>

#include <vine/math/Matrix4x4.hpp>
#include <vine/math/Transform3.hpp>

using namespace vine::math;

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

TEST(Matrix4x4, isIdentity) {
    Mat4d m;
    ASSERT_TRUE(m.isIdentity());
}

TEST(Matrix4x4, setBasis_getBasis) {
    Mat4d m;
    Point3d o(1.0, 2.0, 3.0);
    Vec3d x(1.0, 0.0, 0.0);
    Vec3d y(0.0, 1.0, 0.0);
    Vec3d z(0.0, 0.0, 1.0);

    setBasis(m, o, x, y, z);

    Point3d o2;
    Vec3d x2, y2, z2;
    getBasis(m, o2, x2, y2, z2);

    EXPECT_DOUBLE_EQ(o2.x, 1.0);
    EXPECT_DOUBLE_EQ(o2.y, 2.0);
    EXPECT_DOUBLE_EQ(o2.z, 3.0);
    EXPECT_DOUBLE_EQ(x2.x, 1.0);
    EXPECT_DOUBLE_EQ(x2.y, 0.0);
    EXPECT_DOUBLE_EQ(x2.z, 0.0);
    EXPECT_DOUBLE_EQ(y2.x, 0.0);
    EXPECT_DOUBLE_EQ(y2.y, 1.0);
    EXPECT_DOUBLE_EQ(y2.z, 0.0);
    EXPECT_DOUBLE_EQ(z2.x, 0.0);
    EXPECT_DOUBLE_EQ(z2.y, 0.0);
    EXPECT_DOUBLE_EQ(z2.z, 1.0);
}

TEST(Matrix4x4, multiplyAssign) {
    Mat4d a;
    makeTranslation(a, 1.0, 2.0, 3.0);

    Mat4d b;
    makeScale(b, 2.0, 3.0, 4.0);

    Mat4d expect = a * b;
    a *= b;

    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            EXPECT_DOUBLE_EQ(a(r, c), expect(r, c));
        }
    }
}

TEST(Matrix4x4, makeRotationZeroAngleMakesIdentity) {
    Mat4d m;
    makeTranslation(m, 9.0, 8.0, 7.0);
    makeRotation(m, Vec3d(1.0, 0.0, 0.0), 0.0);
    EXPECT_TRUE(m.isIdentity());
}

TEST(Matrix4x4, transformVectorAndPoint) {
    Mat4d m;
    makeTranslation(m, 10.0, 20.0, 30.0);

    Vec3d v(1.0, 2.0, 3.0);
    Point3d p(1.0, 2.0, 3.0);

    // Vector should ignore translation.
    Vec3d v2 = m * v;
    EXPECT_DOUBLE_EQ(v2.x, 1.0);
    EXPECT_DOUBLE_EQ(v2.y, 2.0);
    EXPECT_DOUBLE_EQ(v2.z, 3.0);

    // Point should include translation.
    Point3d p2 = m * p;
    EXPECT_DOUBLE_EQ(p2.x, 11.0);
    EXPECT_DOUBLE_EQ(p2.y, 22.0);
    EXPECT_DOUBLE_EQ(p2.z, 33.0);
}

TEST(Matrix4x4, invertAffineRoundTrip) {
    constexpr double pi = 3.14159265358979323846;

    Mat4d t;
    makeTranslation(t, 5.0, -2.0, 1.5);

    Mat4d r;
    makeRotation(r, Vec3d(0.0, 0.0, 1.0), pi * 0.25);

    Mat4d s;
    makeScale(s, 2.0, 3.0, 4.0);

    Mat4d m = t * r * s;
    Mat4d inv = m.inverted();
    Mat4d id = m * inv;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (i == j) {
                EXPECT_NEAR(id(i, j), 1.0, 1e-9);
            }
            else {
                EXPECT_NEAR(id(i, j), 0.0, 1e-9);
            }
        }
    }
}

TEST(Matrix4x4, quaternionRotationMatchesAxisAngle) {
    constexpr double half_pi = 1.5707963267948966;
    Quatd q(half_pi, Vec3d(0.0, 0.0, 1.0));

    Mat4d mq;
    makeRotation(mq, q);

    Mat4d ma;
    makeRotation(ma, Vec3d(0.0, 0.0, 1.0), half_pi);

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_NEAR(mq(i, j), ma(i, j), 1e-12);
        }
    }
}

TEST(Matrix4x4, rotationExtractsQuaternionRoundTrip) {
    constexpr double kTol = 1e-12;

    const Quatd source(0.73, Vec3d(1.0, -2.0, 0.5));

    Mat4d m;
    makeRotation(m, source);

    const Quatd extracted = getRotation(m);

    Mat4d reconstructed;
    makeRotation(reconstructed, extracted);

    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            EXPECT_NEAR(reconstructed(r, c), m(r, c), kTol);
        }
    }
}

TEST(Matrix4x4, rotationIgnoresNonUniformScale) {
    constexpr double kTol = 1e-12;

    const Quatd source(1.1, Vec3d(-1.0, 2.0, 0.25));

    Mat4d pure_rotation;
    makeRotation(pure_rotation, source);

    Mat4d scaled_rotation = pure_rotation;
    postScale(scaled_rotation, Vec3d(2.5, 0.5, 3.0));

    const Quatd extracted = getRotation(scaled_rotation);

    Mat4d reconstructed;
    makeRotation(reconstructed, extracted);

    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            EXPECT_NEAR(reconstructed(r, c), pure_rotation(r, c), kTol);
        }
    }
}

TEST(Matrix4x4, isRigidRejectsScaleAndReflection) {
    Mat4d rigid;
    makeRotation(rigid, Vec3d(0.0, 1.0, 0.0), 0.3);
    rigid(0, 3) = 1.0;
    rigid(1, 3) = 2.0;
    rigid(2, 3) = 3.0;
    EXPECT_TRUE(rigid.isRigid());

    Mat4d scaled;
    makeScale(scaled, 2.0, 1.0, 1.0);
    EXPECT_FALSE(scaled.isRigid());

    Mat4d reflected;
    makeScale(reflected, -1.0, 1.0, 1.0);
    EXPECT_FALSE(reflected.isRigid());
}

TEST(Matrix4x4, perspectiveMapsPointWithHomogeneousDivide) {
    constexpr double pi = 3.14159265358979323846;
    Mat4d p;
    makePerspective(p, pi / 2.0, 1.0, 1.0, 100.0);

    Point3d view_pt(0.0, 0.0, -2.0);
    Point3d clip_ndc = p * view_pt;

    // Center line should stay centered after perspective divide.
    EXPECT_NEAR(clip_ndc.x, 0.0, 1e-12);
    EXPECT_NEAR(clip_ndc.y, 0.0, 1e-12);

    // z in OpenGL NDC should be in [-1, 1] for point inside frustum.
    EXPECT_GE(clip_ndc.z, -1.0);
    EXPECT_LE(clip_ndc.z, 1.0);
}

TEST(Matrix4x4, prePostOperationsMatchMatrixMultiplication) {
    constexpr double kTol = 1e-12;

    auto expectMatNear = [&](const Mat4d& lhs, const Mat4d& rhs) {
        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                EXPECT_NEAR(lhs(r, c), rhs(r, c), kTol);
            }
        }
    };

    Mat4d base;
    Mat4d tr;
    makeTranslation(tr, 1.2, -3.4, 5.6);
    Mat4d rot;
    makeRotation(rot, Vec3d(1.0, 2.0, 3.0), 0.47);
    Mat4d sc;
    makeScale(sc, 0.8, 1.3, 2.1);
    base = tr * rot * sc;

    const Vec3d axis(2.0, -1.0, 0.5);
    const double angle = 0.33;
    const Quatd q(angle, axis);
    const Vec3d offset(7.0, -8.0, 9.0);
    const Vec3d factor(1.5, 0.6, 1.2);

    Mat4d rmat;
    makeRotation(rmat, q);
    Mat4d tmat;
    makeTranslation(tmat, offset);
    Mat4d smat;
    makeScale(smat, factor);

    {
        Mat4d m = base;
        preRotate(m, q);
        expectMatNear(m, rmat * base);
    }
    {
        Mat4d m = base;
        postRotate(m, q);
        expectMatNear(m, base * rmat);
    }
    {
        Mat4d m = base;
        preTranslate(m, offset);
        expectMatNear(m, tmat * base);
    }
    {
        Mat4d m = base;
        postTranslate(m, offset);
        expectMatNear(m, base * tmat);
    }
    {
        Mat4d m = base;
        preScale(m, factor);
        expectMatNear(m, smat * base);
    }
    {
        Mat4d m = base;
        postScale(m, factor);
        expectMatNear(m, base * smat);
    }
}