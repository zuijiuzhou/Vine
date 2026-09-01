#pragma once
#include "graphics_global.hpp"

#include <vine/Color.hpp>
#include <vector>

V_GRAPHICS_NS_BEGIN

class Camera;
class RenderTarget;
class RenderPass;
struct RenderCommand;

/**
 * @brief Abstract render backend interface.
 *
 * Defines the contract that concrete graphics backends (OpenGL, Vulkan, etc.)
 * must implement. Supports both high-level pass execution and low-level
 * command rendering.
 */
class V_GRAPHICS_API RenderBackend {
  public:
    virtual ~RenderBackend() = default;

    /** @brief Initializes the backend. */
    virtual bool initialize() = 0;

    /** @brief Releases backend resources. */
    virtual void shutdown() = 0;

    /** @brief Begins a frame. */
    virtual void beginFrame() = 0;

    /** @brief Ends a frame. */
    virtual void endFrame() = 0;

    /** @brief Executes a single render pass.
     *
     * @param pass     The render pass to execute.
     * @param commands Render commands collected for the pass.
     */
    virtual void executePass(const RenderPass* pass,
                             const std::vector<RenderCommand>& commands) = 0;

    /** @brief Sets the active render target.
     *
     * @param target Render target, or nullptr for the default framebuffer.
     */
    virtual void setRenderTarget(RenderTarget* target) = 0;

    /** @brief Renders a list of commands.
     *
     * @param commands Render commands to draw.
     * @param camera   Camera used for view/projection.
     */
    virtual void render(const std::vector<RenderCommand>& commands,
                        const Camera* camera) = 0;

    /** @brief Clears buffers.
     *
     * @param backgroundColor Clear color.
     * @param clearDepth      Whether to also clear the depth buffer.
     */
    virtual void clear(const Color& backgroundColor, bool clearDepth = true) = 0;

    /** @brief Swaps buffers (double buffering). */
    virtual void swapBuffers() = 0;

  protected:
    RenderBackend() = default;
};

V_GRAPHICS_NS_END
