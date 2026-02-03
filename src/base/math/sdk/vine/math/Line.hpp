#pragma once 
#include "Point3.hpp"
#include "Vector3.hpp"

VI_MATH_NS_BEGIN

/**
 * @brief
 * @tparam T Only accepts float and double.
 */
template <typename T>
class Line {
  public:
    using value_type = T;

    struct Intersection {
        // 是否相交
        bool is_intersected{ false };

        // 是否平行
        bool is_parallel{ false };

        // 是否重合
        bool is_collinear{ false };

        // 最近点的参数信息
        struct {

            // 最近点1的参数(点向式)
            // 平行时：为0
            T t1{ 0 };

            // 最近点2的参数(点向式)
            // 平行时：L1的origin在L2上的最近点的参数
            T t2{ 0 };

            // 最近点1
            // 平行时：等于L1的origin
            Point3<T> pt1;

            // 最近点2
            // 平行时：L1的origin在L2上的最近点
            Point3<T> pt2;

            // 最近点间的距离
            T distance{ 0 };

        } params;
    };

  public:
    Line(const Point3<T>& origin, const Vector3<T>& direction);

  public:
    Intersection intersectWith(const Line& other, T eps) const;

    Point3<T> closestPoint(const Point3<T>& pt) const;

  public:
    Point3<T>  origin;
    Vector3<T> direction;
};

using Linef = Line<float>;
using Lined = Line<double>;

VI_MATH_NS_END