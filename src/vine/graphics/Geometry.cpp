#include <vine/graphics/Geometry.h>

VI_GRAPHICS_NS_BEGIN

VI_OBJECT_META_IMPL(Geometry, Object)

struct Geometry::Data {};

Geometry::Geometry() : d(new Data()) {}

VI_GRAPHICS_NS_END
