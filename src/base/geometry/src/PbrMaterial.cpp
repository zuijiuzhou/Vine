#include <vine/geometry/PbrMaterial.hpp>

V_GEOMETRY_NS_BEGIN

V_OBJECT_META_IMPL(PbrMaterial, Material)

PbrMaterial::PbrMaterial()
{
    material_type_ = MaterialType::Pbr;
}

PbrMaterial::PbrMaterial(const vine::Colorf& base_color, float metallic, float roughness, float opacity)
  : base_color_(base_color)
  , metallic_(metallic)
  , roughness_(roughness)
  , opacity_(opacity)
{
    material_type_ = MaterialType::Pbr;
}

const vine::Colorf& PbrMaterial::baseColor() const
{
    return base_color_;
}

void PbrMaterial::setBaseColor(const vine::Colorf& color)
{
    base_color_ = color;
}

const vine::Colorf& PbrMaterial::emissive() const
{
    return emissive_;
}

void PbrMaterial::setEmissive(const vine::Colorf& color)
{
    emissive_ = color;
}

float PbrMaterial::metallic() const
{
    return metallic_;
}

void PbrMaterial::setMetallic(float metallic)
{
    metallic_ = metallic;
}

float PbrMaterial::roughness() const
{
    return roughness_;
}

void PbrMaterial::setRoughness(float roughness)
{
    roughness_ = roughness;
}

float PbrMaterial::opacity() const
{
    return opacity_;
}

void PbrMaterial::setOpacity(float opacity)
{
    opacity_ = opacity;
}

const char* PbrMaterial::typeName() const
{
    return "PbrMaterial";
}

V_GEOMETRY_NS_END
