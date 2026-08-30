#include <vine/IntrusivePtr.hpp>
#include <vine/graphics/Scene.hpp>
#include <vine/graphics/View.hpp>

V_GRAPHICS_NS_BEGIN

V_OBJECT_META_IMPL(View, vine::Object)

struct View::Data {
    IntrusivePtr<Scene> scene;
};

View::View()
  : d(new Data())
{}

void View::setScene(Scene* scene)
{
    if (scene == d->scene)
        return;
    d->scene = scene;
}

V_GRAPHICS_NS_END
