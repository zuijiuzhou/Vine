#pragma once

#include "etd_ge_export.h"

namespace etd
{
    namespace ge
    {
        class Point2d;
        class Vector2d;
        class ETD_GE_API Matrix2d
        {
        public:
            double data[3][3];

            Matrix2d();

        public:
            void makeIdentity();
            void makeRotation(const Point2d &center, double angle);
            void makeTranslation(const Vector2d &offset);
            void makeTranslation(double x, double y);
            void makeScale(const Vector2d &vec);
            void makeScale(double x, double y);
            void makeScale(double factor);
            void setCoordSystem(const Point2d &origin, const Vector2d &xAxis, const Vector2d &yAxis);
            void transpose();
            void invert();

            bool isIdentity() const;

        public:
            double operator()(int row, int col) const;
            double &operator()(int row, int col);
            Matrix2d operator*(const Matrix2d &right) const;
            Matrix2d &operator*=(const Matrix2d &right) const;

        public:
            static Matrix2d rotate(const Point2d &center, double angle);
            static Matrix2d translate(const Vector2d &offset);
            static Matrix2d translate(double x, double y);
            static Matrix2d scale(const Vector2d &vec);
            static Matrix2d scale(double x, double y);
            static Matrix2d scale(double factor);
        };
    }
}