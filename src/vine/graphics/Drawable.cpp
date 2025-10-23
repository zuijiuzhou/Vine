#include <vine/graphics/Drawable.hpp>

VI_GRAPHICS_NS_BEGIN

VI_OBJECT_META_IMPL(Drawable, RefObject)

struct Drawable::Data {};

Drawable::Drawable() : d(new Data()) {}

VI_GRAPHICS_NS_END
