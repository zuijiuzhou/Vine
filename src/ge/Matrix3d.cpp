#include <ge/Matrix3d.h>

#include <memory>

namespace etd
{
    namespace ge
    {
        Matrix3d::Matrix3d()
        {
            makeIdentity();
        }
        void Matrix3d::makeIdentity()
        {
            data[2][3] = 4;
            data[1][2] = 5;
            // std::fill((double*)data, ((double*)data)+16, 0.0);
            memset(data, 0, sizeof(data));
            data[0][0] = data[1][1] = data[2][2] = data[3][3] = 1.0;
        }
        void Matrix3d::makeRotation(const Point3d &center, const Vector3d &axis, double angle)
        {
            
        }
        void Matrix3d::makeTranslation(const Vector3d &offset)
        {

        }
        void Matrix3d::makeTranslation(double x, double y, double z)
        {

        }
        void Matrix3d::makeLookAt(const Point3d &eye, const Point3d &target, const Vector3d &up)
        {

        }
        void Matrix3d::setCoordSystem(const Point3d& origin, const Vector3d &xAxis, const Vector3d &yAxis, const Vector3d &zAxis)
        {

        }
        void Matrix3d::transpose()
        {
            std::swap(data[0][1], data[1][0]);
            std::swap(data[0][2], data[2][0]);
            std::swap(data[0][3], data[3][0]);

            std::swap(data[1][2], data[2][1]);
            std::swap(data[1][3], data[3][1]);

            std::swap(data[2][3], data[3][2]);
        }
        void Matrix3d::invert()
        {
        }

        bool Matrix3d::isAffine() const
        {
            return false;
        }
        bool Matrix3d::isIdentity() const
        {
            return true;
        }
        double Matrix3d::operator()(int row, int col) const
        {
            return data[row][col];
        }
        double &Matrix3d::operator()(int row, int col)
        {
            return data[row][col];
        }
        Matrix3d Matrix3d::operator*(const Matrix3d &right) const
        {
            Matrix3d m;
            return m;
        }
        Matrix3d &Matrix3d::operator*=(const Matrix3d &right) const
        {
            Matrix3d m;
            return const_cast<Matrix3d &>(*this);
        }

        Matrix3d Matrix3d::rotate(const Point3d &center, const Vector3d &axis, double angle)
        {
            Matrix3d m;
            m.makeRotation(center, axis, angle);
            return m;
        }
        Matrix3d Matrix3d::translate(const Vector3d &offset)
        {
            Matrix3d m;
            m.translate(offset);
            return m;
        }
        Matrix3d Matrix3d::translate(double x, double y, double z)
        {
            Matrix3d m;
            m.translate(x, y, z);
            return m;
        }
        Matrix3d Matrix3d::scale(const Vector3d &vec)
        {
            Matrix3d m;
            m.scale(vec);
            return m;
        }
        Matrix3d Matrix3d::scale(double x, double y, double z)
        {
            Matrix3d m;
            m.scale(x, y, z);
            return m;
        }
        Matrix3d Matrix3d::scale(double factor)
        {
            Matrix3d m;
            m.scale(factor);
            return m;
        }
        Matrix3d Matrix3d::lookAt(const Point3d &eye, const Point3d &target, const Vector3d &up)
        {
            Matrix3d m;
            m.lookAt(eye, target, up);
            return m;
        }
    }
}
