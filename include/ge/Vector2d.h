#pragma once

#include "etd_ge_export.h"


namespace etd
{
    namespace ge
    {
        class Point2d;
        class ETD_GE_API Vector2d
        {
        public:
            Vector2d();
            Vector2d(double xx, double yy);

        public:
            double dotProduct(const Vector2d& vec) const;
            double crossProduct(const Vector2d& vec) const;
            void normalize();
            Vector2d perpVector() const;

            double length() const;
            double lengthSqrd() const;

            void setLength(double len);
            void set(double xx, double yy, double zz);
            void get(double& xx, double& yy, double& zz) const;
            // void rotate(const Matrix2d& mat);
            double angleTo(const Vector2d& vec);

            bool isZeroLength() const;
            bool isParalleTo(const Vector2d& vec) const;
            bool isPerpendicularTo(const Vector2d& vec) const;
            
            Point2d toPoint() const;
            const Point2d& asPoint();
            Vector2d convert2d() const;

        public:
            bool operator==(const Vector2d &right) const;
            bool operator!=(const Vector2d &right) const;
            Vector2d operator-(const Vector2d &right) const;
            Vector2d operator+(const Vector2d &right) const;
            Vector2d &operator+=(const Vector2d &right);
            Vector2d &operator-=(const Vector2d &right);
            
            Vector2d operator*(double scale) const;
            Vector2d& operator*=(double scale);            
            Vector2d operator/(double scale) const;
            Vector2d& operator/=(double scale);

            double operator^(const Vector2d &vec) const;
            double operator*(const Vector2d &vec) const;

        public:
            double x;
            double y;
        };
    }
}