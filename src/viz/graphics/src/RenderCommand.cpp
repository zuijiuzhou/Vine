#include <vine/graphics/RenderCommand.hpp>

#include <vine/graphics/Drawable.hpp>
#include <vine/graphics/Material.hpp>

V_GRAPHICS_NS_BEGIN

RenderCommand::RenderCommand(intrusive_ptr<Drawable> d, intrusive_ptr<Material> m,
                             const Mat4d& model)
  : drawable(std::move(d))
  , material(std::move(m))
  , modelMatrix(model)
{
    if (material != nullptr) {
        opacity = material->opacity();
        if (opacity < 1.0f) {
            isTransparent = true;
        }
    }
}

V_GRAPHICS_NS_END
