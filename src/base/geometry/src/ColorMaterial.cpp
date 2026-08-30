#include <vine/geometry/ColorMaterial.hpp>

V_GEOMETRY_NS_BEGIN

V_OBJECT_META_IMPL(ColorMaterial, Material)

ColorMaterial::ColorMaterial()
{
    material_type_ = MaterialType::Color;
}

ColorMaterial::ColorMaterial(const vine::Colorf& color)
  : color_(color)
{
    material_type_ = MaterialType::Color;
}

const vine::Colorf& ColorMaterial::color() const
{
    return color_;
}

void ColorMaterial::setColor(const vine::Colorf& color)
{
    color_ = color;
}

const char* ColorMaterial::typeName() const
{
    return "ColorMaterial";
}

V_GEOMETRY_NS_END
