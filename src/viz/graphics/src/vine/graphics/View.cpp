#include <vine/Ptr.hpp>
#include <vine/graphics/Scene.hpp>
#include <vine/graphics/View.hpp>

V_GRAPHICS_NS_BEGIN

V_OBJECT_META_IMPL(View, RefObject)

struct View::Data {
    RefPtr<Scene> scene;
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
