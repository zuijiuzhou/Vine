#include <vine/geometry/PhongMaterial.hpp>

V_GEOMETRY_NS_BEGIN

V_OBJECT_META_IMPL(PhongMaterial, Material)

PhongMaterial::PhongMaterial()
{
    material_type_ = MaterialType::Phong;
}

PhongMaterial::PhongMaterial(const vine::Colorf& ambient, const vine::Colorf& diffuse,
    const vine::Colorf& specular, float shininess)
  : ambient_(ambient)
  , diffuse_(diffuse)
  , specular_(specular)
  , shininess_(shininess)
{
    material_type_ = MaterialType::Phong;
}

const vine::Colorf& PhongMaterial::ambient() const
{
    return ambient_;
}

void PhongMaterial::setAmbient(const vine::Colorf& ambient)
{
    ambient_ = ambient;
}

const vine::Colorf& PhongMaterial::diffuse() const
{
    return diffuse_;
}

void PhongMaterial::setDiffuse(const vine::Colorf& diffuse)
{
    diffuse_ = diffuse;
}

const vine::Colorf& PhongMaterial::specular() const
{
    return specular_;
}

void PhongMaterial::setSpecular(const vine::Colorf& specular)
{
    specular_ = specular;
}

float PhongMaterial::shininess() const
{
    return shininess_;
}

void PhongMaterial::setShininess(float shininess)
{
    shininess_ = shininess;
}

const char* PhongMaterial::typeName() const
{
    return "PhongMaterial";
}

V_GEOMETRY_NS_END
