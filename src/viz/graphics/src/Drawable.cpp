#include <vine/graphics/Drawable.hpp>

#include <vine/graphics/Material.hpp>

V_GRAPHICS_NS_BEGIN

V_OBJECT_META_IMPL(Drawable, vine::Object);

struct Drawable::Data {
    String name;
    intrusive_ptr<Material> material;
};

Drawable::Drawable()
  : d(new Data())
{}

Drawable::~Drawable()
{
    delete d;
}

String Drawable::name() const
{
    return d->name;
}

void Drawable::setName(const String& name)
{
    d->name = name;
}

BoundingBox Drawable::boundingBox() const
{
    return computeBoundingBox();
}

Material* Drawable::material() const
{
    return d->material.get();
}

void Drawable::setMaterial(Material* m)
{
    d->material = m;
}

V_GRAPHICS_NS_END
