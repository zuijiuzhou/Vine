#pragma once
#include "graphics_global.hpp"

#include <vine/intrusive_ptr.hpp>
#include <vine/Object.hpp>
#include <vine/RefCounted.hpp>
#include <vine/Color.hpp>
#include <string>
#include <vector>

V_GRAPHICS_NS_BEGIN

class Camera;
class RenderTarget;
class RenderBackend;
class Scene;

/**
 * @brief A render pass describing one complete rendering stage.
 *
 * Binds a camera, render target, and clear state. Executing a pass
 * collects render commands from a scene and dispatches them to a backend.
 * Multiple passes can be chained for split-screen, post-processing, etc.
 */
class V_GRAPHICS_API RenderPass : public Object, public RefCounted<RenderPass> {
    V_OBJECT_META_DECL;

  public:
    RenderPass();

  public:
    /** @brief Gets the pass name. */
    String name() const;

    /** @brief Sets the pass name. */
    void setName(const String& name);

    /** @brief Gets the associated render target. */
    RenderTarget* renderTarget() const;

    /** @brief Sets the render target.
     *
     * @param target Render target to render into.
     */
    void setRenderTarget(RenderTarget* target);

    /** @brief Gets the camera used by this pass. */
    Camera* camera() const;

    /** @brief Sets the camera used by this pass. */
    void setCamera(Camera* camera);

    /** @brief Gets the clear color. */
    Color clearColor() const;

    /** @brief Sets the clear color. */
    void setClearColor(const Color& color);

    /** @brief Returns whether the depth buffer is cleared. */
    bool shouldClearDepth() const;

    /** @brief Sets whether the depth buffer is cleared. */
    void setShouldClearDepth(bool clear);

    /** @brief Executes this render pass.
     *
     * @param scene   Scene containing drawables.
     * @param backend Backend to render with.
     */
    void execute(Scene* scene, RenderBackend* backend);

  private:
    struct Data;
    Data* const d;
};

using RenderPassPtr = intrusive_ptr<RenderPass>;

V_GRAPHICS_NS_END
