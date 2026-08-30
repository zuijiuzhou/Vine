#include <vine/graphics/Scene.hpp>

V_GRAPHICS_NS_BEGIN

V_OBJECT_META_IMPL(Scene, vine::Object);

struct Scene::Data {};

Scene::Scene()
  : d(new Data())
{}

V_GRAPHICS_NS_END
