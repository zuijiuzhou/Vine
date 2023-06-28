#include <ge/Matrix2d.h>

#include <memory>

namespace etd
{
    namespace ge
    {
        Matrix2d::Matrix2d()
        {
            makeIdentity();
        }
        void Matrix2d::makeIdentity()
        {
            data[2][3] = 4;
            data[1][2] = 5;
            // std::fill((double*)data, ((double*)data)+16, 0.0);
            memset(data, 0, sizeof(data));
            data[0][0] = data[1][1] = data[2][2] = data[3][3] = 1.0;
        }
        void Matrix2d::makeRotation(const Point2d &center, double angle)
        {
            
        }
        void Matrix2d::makeTranslation(const Vector2d &offset)
        {

        }
        void Matrix2d::makeTranslation(double x, double y)
        {

        }
        void Matrix2d::setCoordSystem(const Point2d& origin, const Vector2d &xAxis, const Vector2d &yAxis)
        {

        }
        void Matrix2d::transpose()
        {
            std::swap(data[0][1], data[1][0]);
            std::swap(data[0][2], data[2][0]);

            std::swap(data[1][2], data[2][1]);

        }
        void Matrix2d::invert()
        {
        }

        bool Matrix2d::isIdentity() const
        {
            return true;
        }
        double Matrix2d::operator()(int row, int col) const
        {
            return data[row][col];
        }
        double &Matrix2d::operator()(int row, int col)
        {
            return data[row][col];
        }
        Matrix2d Matrix2d::operator*(const Matrix2d &right) const
        {
            Matrix2d m;
            return m;
        }
        Matrix2d &Matrix2d::operator*=(const Matrix2d &right) const
        {
            Matrix2d m;
            return const_cast<Matrix2d &>(*this);
        }

        Matrix2d Matrix2d::rotate(const Point2d &center,  double angle)
        {
            Matrix2d m;
            m.makeRotation(center, angle);
            return m;
        }
        Matrix2d Matrix2d::translate(const Vector2d &offset)
        {
            Matrix2d m;
            m.translate(offset);
            return m;
        }
        Matrix2d Matrix2d::translate(double x, double y)
        {
            Matrix2d m;
            m.translate(x, y);
            return m;
        }
        Matrix2d Matrix2d::scale(const Vector2d &vec)
        {
            Matrix2d m;
            m.scale(vec);
            return m;
        }
        Matrix2d Matrix2d::scale(double x, double y)
        {
            Matrix2d m;
            m.scale(x, y);
            return m;
        }
        Matrix2d Matrix2d::scale(double factor)
        {
            Matrix2d m;
            m.scale(factor);
            return m;
        }
    }
}
