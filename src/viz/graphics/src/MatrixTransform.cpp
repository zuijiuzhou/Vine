#include <vine/graphics/MatrixTransform.hpp>

V_GRAPHICS_NS_BEGIN

V_OBJECT_META_IMPL(MatrixTransform, Group);

MatrixTransform::MatrixTransform() = default;

MatrixTransform::~MatrixTransform() = default;

Mat4d MatrixTransform::matrix() const
{
    return matrix_;
}

void MatrixTransform::setMatrix(const Mat4d& matrix)
{
    matrix_ = matrix;
}

Mat4d MatrixTransform::localTransformMatrix() const
{
    return matrix_;
}

V_GRAPHICS_NS_END

