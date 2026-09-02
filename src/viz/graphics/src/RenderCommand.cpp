#include <vine/graphics/RenderCommand.hpp>

#include <vine/graphics/Drawable.hpp>
#include <vine/graphics/Material.hpp>

V_GRAPHICS_NS_BEGIN

RenderCommand::RenderCommand(Drawable* d, Material* m, const Mat4d& model)
  : drawable(d)
  , material(m)
  , modelMatrix(model)
{
    if (m != nullptr) {
        opacity = m->opacity();
        if (opacity < 1.0f) {
            isTransparent = true;
        }
    }
}

V_GRAPHICS_NS_END
