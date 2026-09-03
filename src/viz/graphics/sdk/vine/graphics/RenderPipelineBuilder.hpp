#pragma once
#include "graphics_global.hpp"

#include <vector>

#include <vine/raw_ptr.hpp>
#include <vine/intrusive_ptr.hpp>

#include "RenderPass.hpp"
#include "RenderTarget.hpp"

V_GRAPHICS_NS_BEGIN

class Camera;
class RenderEngine;
class Scene;
class ScreenPass;

/**
 * @brief Thin, ergonomic recipe layer for assembling a render pipeline.
 *
 * RenderPipelineBuilder produces the exact same objects you would build by
 * hand through the RenderEngine / RenderPass public API, but packages the
 * recurring "recipes" (off-screen render-to-texture + screen compositing,
 * later shadowed-lit scenes, ...) so applications do not repeat the wiring.
 *
 * It is intentionally thin: it does NOT own the per-frame scheduling,
 * content resolution, lighting or shadow logic — those stay in RenderEngine
 * (the builder only fills in order / content / output / input names). Every
 * add*() call applies immediately to the target engine, and created passes
 * are kept alive both by the builder and by the engine.
 *
 * RenderEngine itself auto-registers nothing (its pipeline is fully
 * explicit); this builder is the convenient, reusable way to assemble common
 * configurations such as an off-screen render-to-texture + screen
 * compositing chain.
 */
class V_GRAPHICS_API RenderPipelineBuilder {
  public:
    /** @brief Constructs a builder targeting an engine.
     *
     * @param engine Engine the assembled passes are applied to (borrowed;
     *               must outlive the builder).
     */
    explicit RenderPipelineBuilder(raw_ptr<RenderEngine> engine);

    /** @brief Binds the content scene for produced scene passes.
     *
     * @param content Scene used by the produced passes, or null to fall back
     *                to the engine's default content scene.
     */
    RenderPipelineBuilder& setContent(intrusive_ptr<Scene> content);

    /** @brief Binds the camera used by the produced scene passes.
     *
     * @param camera Camera (borrowed), or null to use the engine master
     *               camera.
     */
    RenderPipelineBuilder& setCamera(raw_ptr<Camera> camera);

    /** @brief Recipe: render content into an off-screen target and composite
     * it back as a picture-in-picture screen pass.
     *
     * Creates:
     *   - an order < 0 scene pass rendering into a @p rt_width x @p rt_height
     *     RenderTarget (publishing its colour as @p output_slot), and
     *   - an order > 0 ScreenPass that samples @p output_slot into the
     *     @p pip_... sub-viewport.
     * Both are added to the engine immediately; the returned ScreenPass lets
     * the caller re-anchor the PiP viewport once the surface size is known
     * (see RenderPass::setViewport). The pass keeps the target alive.
     *
     * @param output_slot Name the off-screen target is published under and
     *                    the screen pass resolves.
     * @param rt_width    Off-screen target width.
     * @param rt_height   Off-screen target height.
     * @param color_format Off-screen colour format.
     * @param depth_format Off-screen depth format.
     * @param pip_x/y/w/h  PiP sub-viewport on the output surface (device px).
     * @return The created ScreenPass (owned by the engine; do not delete).
     */
    raw_ptr<ScreenPass> addOffscreenToScreen(const String& output_slot,
                                             int rt_width, int rt_height,
                                             RenderTarget::ColorFormat color_format,
                                             RenderTarget::DepthFormat depth_format,
                                             int pip_x, int pip_y, int pip_w, int pip_h);

    /** @brief Manual escape hatch: adds an arbitrary pass to the engine.
     *
     * @param pass  Pass to add (the engine keeps a reference).
     * @param order Execution order relative to the main pass.
     */
    void addPass(intrusive_ptr<RenderPass> pass, int order);

  private:
    raw_ptr<RenderEngine>      engine_;
    raw_ptr<Camera>            camera_      = nullptr;
    intrusive_ptr<Scene>       content_;
    // References kept by the builder for as long as it lives; the engine also
    // holds its own references after each add*() call.
    std::vector<intrusive_ptr<RenderPass>> passes_;
};

V_GRAPHICS_NS_END
