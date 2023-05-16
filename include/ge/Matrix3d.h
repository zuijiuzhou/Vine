#pragma once

#include "etd_ge_export.h"

namespace etd
{
    namespace ge
    {
        class Point3d;
        class Vector3d;
        class ETD_GE_EXPORT Matrix3d
        {
        public:
            double data[4][4];

        public:
            Matrix3d();

        public:
            void makeIdentity();
            void makeRotation(const Point3d &center, const Vector3d &axis, double angle);
            void makeTranslation(const Vector3d &offset);
            void makeTranslation(double x, double y, double z);
            void makeScale(const Vector3d& vec);
            void makeScale(double x, double y, double z);
            void makeScale(double factor);
            void makeLookAt(const Point3d &eye, const Point3d &target, const Vector3d &up);
            void setCoordSystem(const Vector3d &xAxis, const Vector3d &yAxis, const Vector3d &zAxis);
            void transpose();
            void invert();

            bool isAffine() const;
            bool isIdentity() const;

        public:
            double operator()(int row, int col) const;
            double &operator()(int row, int col);
            Matrix3d operator*(const Matrix3d &right) const;
            Matrix3d &operator*=(const Matrix3d &right) const;

        public:
            static Matrix3d rotate(const Point3d &center, const Vector3d &axis, double angle);
            static Matrix3d translate(const Vector3d &offset);
            static Matrix3d translate(double x, double y, double z);
            static Matrix3d scale(const Vector3d& vec);
            static Matrix3d scale(double x, double y, double z);
            static Matrix3d scale(double factor);
            static Matrix3d lookAt(const Point3d &eye, const Point3d &target, const Vector3d &up);
        };
    }
}