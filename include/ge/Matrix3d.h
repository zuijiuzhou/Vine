#pragma once

#include "etd_ge_export.h"

namespace etd{
    namespace ge{
        class Point3d;
        class Vector3d;
        class ETD_GE_API Matrix3d{
            public:
                double data[4][4];

                public:
                Matrix3d();

                public:
                void setToIdentity();
                void setToRotation(const Point3d& center, const Vector3d& axis, double angle);
                void setToTranslation(const Vector3d& offset);
                void setToTranslation(double x, double y, double z);
                void setLookAt(const Point3d& eye, const Point3d& target, const Vector3d& up);
                void setCoordSystem(const Vector3d& xAxis, const Vector3d& yAxis, const Vector3d& zAxis);
                void transpose();
                void invert();
                public:
                double operator()(int row, int col) const;
                double& operator()(int row, int col);
                Matrix3d operator *(const Matrix3d& right) const;
                Matrix3d& operator *=(const Matrix3d& right) const;
        };
    }
}