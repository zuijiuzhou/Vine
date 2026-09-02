#include <vine/graphics/Drawable.hpp>

#include <vine/graphics/Material.hpp>

V_GRAPHICS_NS_BEGIN

V_OBJECT_META_IMPL(Drawable, vine::Object);

Drawable::Drawable() = default;

Drawable::~Drawable() = default;

String Drawable::name() const
{
    return name_;
}

void Drawable::setName(const String& name)
{
    name_ = name;
}

Aabbd Drawable::boundingBox() const
{
    return computeBoundingBox();
}

Material* Drawable::material() const
{
    return material_.get();
}

void Drawable::setMaterial(Material* m)
{
    material_ = m;
}

bool Drawable::isVisible() const
{
    return visible_;
}

void Drawable::setVisible(bool visible)
{
    visible_ = visible;
}

float Drawable::opacity() const
{
    return opacity_;
}

void Drawable::setOpacity(float opacity)
{
    opacity_ = opacity;
}

V_GRAPHICS_NS_END
