#include <vine/graphics/Geometry.hpp>

VI_GRAPHICS_NS_BEGIN

VI_OBJECT_META_IMPL(Geometry, RefObject)

struct Geometry::Data {};

Geometry::Geometry() : d(new Data()) {}

VI_GRAPHICS_NS_END
