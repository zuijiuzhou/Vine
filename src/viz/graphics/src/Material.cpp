#include <vine/graphics/Material.hpp>

V_GRAPHICS_NS_BEGIN

V_OBJECT_META_IMPL(Material, vine::Object);

struct Material::Data {
    String name;
    Colorf diffuse{ 0.8f, 0.8f, 0.8f, 1.0f };
    Colorf specular{ 1.0f, 1.0f, 1.0f, 0.5f };
    Colorf ambient{ 0.2f, 0.2f, 0.2f, 1.0f };
    float shininess = 32.0f;
    float opacity = 1.0f;
    String texture_file;
};

Material::Material()
  : d(new Data())
{}

String Material::name() const
{
    return d->name;
}

void Material::setName(const String& name)
{
    d->name = name;
}

Colorf Material::diffuse() const
{
    return d->diffuse;
}

void Material::setDiffuse(const Colorf& color)
{
    d->diffuse = color;
}

Colorf Material::specular() const
{
    return d->specular;
}

void Material::setSpecular(const Colorf& color)
{
    d->specular = color;
}

Colorf Material::ambient() const
{
    return d->ambient;
}

void Material::setAmbient(const Colorf& color)
{
    d->ambient = color;
}

float Material::shininess() const
{
    return d->shininess;
}

void Material::setShininess(float shine)
{
    d->shininess = shine;
}

float Material::opacity() const
{
    return d->opacity;
}

void Material::setOpacity(float alpha)
{
    d->opacity = alpha;
}

String Material::textureFile() const
{
    return d->texture_file;
}

void Material::setTextureFile(const String& path)
{
    d->texture_file = path;
}

V_GRAPHICS_NS_END
