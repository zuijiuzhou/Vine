#pragma once
#include "graphics_global.hpp"

#include <vine/intrusive_ptr.hpp>
#include <vine/Object.hpp>
#include <vine/RefCounted.hpp>
#include <vine/raw_ptr.hpp>
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
    ~RenderPass();

  public:
    /** @brief Gets the pass name. */
    String name() const;

    /** @brief Sets the pass name. */
    void setName(const String& name);

    /** @brief Gets the associated render target. */
    raw_ptr<RenderTarget> renderTarget() const;

    /** @brief Sets the render target this pass renders into.
     *
     * The pass keeps a reference to the target (its output resource), so the
     * target stays alive as long as the pass uses it.
     *
     * @param target Render target to render into, or null for the default
     *               framebuffer.
     */
    void setRenderTarget(intrusive_ptr<RenderTarget> target);

    /** @brief Gets the camera used by this pass. */
    raw_ptr<Camera> camera() const;

    /** @brief Sets the camera used by this pass. */
    void setCamera(raw_ptr<Camera> camera);

    /** @brief Gets the clear color. */
    Color clearColor() const;

    /** @brief Sets the clear color. */
    void setClearColor(const Color& color);

    /** @brief Returns whether the depth buffer is cleared. */
    bool shouldClearDepth() const;

    /** @brief Sets whether the depth buffer is cleared. */
    void setShouldClearDepth(bool clear);

    /** @brief Returns whether the colour/depth buffer is cleared before this pass.
     *
     * The main pass clears by default; overlay passes usually disable it so
     * they draw over the previous frame's content.
     */
    bool clearEnabled() const;

    /** @brief Sets whether the colour/depth buffer is cleared before this pass.
     *
     * @param enabled True to clear the buffers (the default).
     */
    void setClearEnabled(bool enabled);

    /** @brief Restricts this pass to a sub-rectangle of the render target.
     *
     * Used for sub-viewports such as an axis gizmo in a screen corner. The
     * pass falls back to the full surface when no viewport is set.
     *
     * @param x      Viewport origin x in device pixels.
     * @param y      Viewport origin y in device pixels (top-left origin).
     * @param width  Viewport width in device pixels.
     * @param height Viewport height in device pixels.
     */
    void setViewport(int x, int y, int width, int height);

    /** @brief Returns whether a sub-viewport is configured for this pass. */
    bool hasViewport() const;

    /** @brief Gets the configured sub-viewport in device pixels.
     *
     * Values are only meaningful when hasViewport() is true.
     *
     * @param x      Receives the viewport origin x.
     * @param y      Receives the viewport origin y.
     * @param width  Receives the viewport width.
     * @param height Receives the viewport height.
     */
    void getViewport(int& x, int& y, int& width, int& height) const;

    /** @brief Clears any configured sub-viewport (pass renders to the full surface). */
    void clearViewport();

    /** @brief Sets the name this pass publishes its output under.
     *
     * When a pass renders into a non-null RenderTarget and declares an output
     * name, the engine registers that target in its named-output registry
     * after the pass runs, so later passes can sample it by name without
     * holding a pointer to the producer. Leave empty to publish nothing.
     *
     * @param name Output slot name, or empty to disable publishing.
     */
    void setOutputName(const String& name);

    /** @brief Gets the output slot name this pass publishes under.
     *
     * @return The output name (empty when publishing is disabled).
     */
    String outputName() const;

    /** @brief Adds an input texture slot this pass consumes by name.
     *
     * A consumer declares "I want the texture published as X" without
     * holding a pointer to the producer. The engine resolves each declared
     * name against its named-output registry before execute() and hands the
     * matching targets to resolveInputTextures().
     *
     * @param name Name of a published output to consume.
     */
    void addInputName(const String& name);

    /** @brief Gets the declared input texture slot names.
     *
     * @return The input names, in the order they were added.
     */
    const std::vector<String>& inputNames() const;

    /** @brief Clears all declared input texture slots. */
    void clearInputNames();

    /** @brief Receives the engine-resolved input textures.
     *
     * Called by the engine just before execute() once per frame with the
     * targets resolved from the named-output registry for each declared input
     * (same order as inputNames(); entries stay null when a name is not yet
     * published). The base pass ignores the inputs; subclasses such as
     * ScreenPass consume them. The passed targets are borrowed: the registry
     * keeps them alive while they are published.
     *
     * @param inputs Resolved input targets in inputNames() order.
     */
    virtual void resolveInputTextures(const std::vector<raw_ptr<RenderTarget>>& inputs)
    {
        (void)inputs;
    }

    /** @brief Executes this render pass.
     *
     * @param scene   Scene containing drawables.
     * @param backend Backend to render with.
     */
    virtual void execute(raw_ptr<Scene> scene, raw_ptr<RenderBackend> backend);

  private:
    String name_;
    String output_name_;
    std::vector<String> input_names_;
    intrusive_ptr<RenderTarget> render_target_;
    raw_ptr<Camera> camera_ = nullptr;
    Color clear_color_{ 51, 51, 51, 255 };
    bool clear_depth_ = true;
    bool clear_enabled_ = true;
    bool has_viewport_ = false;
    int viewport_x_ = 0;
    int viewport_y_ = 0;
    int viewport_w_ = 0;
    int viewport_h_ = 0;
};

using RenderPassPtr = intrusive_ptr<RenderPass>;

V_GRAPHICS_NS_END
