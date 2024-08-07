#include <vine/core/Ptr.h>
#include <vine/graphics/Scene.h>
#include <vine/graphics/View.h>

VI_GRAPHICS_NS_BEGIN

VI_OBJECT_META_IMPL(View, Object)

struct View::Data {
    RefPtr<Scene> scene;
};

View::View()
  : d(new Data()) {
}

void View::setScene(Scene* scene) {
    if (scene == d->scene) return;
    d->scene = scene;
}

VI_GRAPHICS_NS_END
