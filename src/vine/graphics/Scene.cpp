#include <vine/graphics/Scene.h>

VI_GRAPHICS_NS_BEGIN

VI_OBJECT_META_IMPL(Scene, Object);

struct Scene::Data {

};

Scene::Scene() : d(new Data()){

}

VI_GRAPHICS_NS_END