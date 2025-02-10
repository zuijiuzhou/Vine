#pragma once

#include "ge_global.h"

VI_GE_NS_BEGIN
class Point2d;
class Vector2d;
class VI_GE_API Matrixd3x3 {
  public:
    Matrixd3x3();

  public:
    void makeIdentity();
    void makeRotation(const Point2d& center, double angle);
    void makeTranslation(const Vector2d& offset);
    void makeTranslation(double x, double y);
    void makeScale(const Vector2d& vec);
    void makeScale(double x, double y);
    void makeScale(double factor);
    void setCoordSystem(const Point2d& origin, const Vector2d& xAxis, const Vector2d& yAxis);
    void transpose();
    void invert();

    bool isIdentity() const;

  public:
    double      operator()(int row, int col) const;
    double&     operator()(int row, int col);
    Matrixd3x3  operator*(const Matrixd3x3& right) const;
    Matrixd3x3& operator*=(const Matrixd3x3& right) const;

  public:
    static Matrixd3x3 rotate(const Point2d& center, double angle);
    static Matrixd3x3 translate(const Vector2d& offset);
    static Matrixd3x3 translate(double x, double y);
    static Matrixd3x3 scale(const Vector2d& vec);
    static Matrixd3x3 scale(double x, double y);
    static Matrixd3x3 scale(double factor);

  public:
    double data[3][3];
};
VI_GE_NS_END