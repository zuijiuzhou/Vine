#include <vine/graphics/Geometry.hpp>

V_GRAPHICS_NS_BEGIN

V_OBJECT_META_IMPL(Geometry, RefObject)

struct Geometry::Data {};

Geometry::Geometry()
  : d(new Data())
{}

V_GRAPHICS_NS_END
