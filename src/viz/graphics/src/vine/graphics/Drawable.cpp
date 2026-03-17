#include <vine/graphics/Drawable.hpp>

V_GRAPHICS_NS_BEGIN

V_OBJECT_META_IMPL(Drawable, RefObject)

struct Drawable::Data {};

Drawable::Drawable()
  : d(new Data())
{}

V_GRAPHICS_NS_END
