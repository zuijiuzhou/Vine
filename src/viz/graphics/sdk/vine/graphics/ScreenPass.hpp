#pragma once
#include "graphics_global.hpp"

#include <vector>

#include <vine/raw_ptr.hpp>

#include "RenderPass.hpp"

V_GRAPHICS_NS_BEGIN

class RenderTarget;

/**
 * @brief A full-screen (screen-space) pass that samples a published texture.
 *
 * A ScreenPass renders no scene geometry: instead it draws a full-screen
 * textured triangle that samples the colour attachment of a source RenderTarget
 * into this pass's output target (or the backbuffer when no target is set).
 * The source is normally resolved by the engine from the named-output registry
 * (see RenderPass::addInputName / resolveInputTextures), so the pass only
 * declares which published texture it consumes. When no input is resolved the
 * pass draws nothing.
 *
 * Typical use: composite an off-screen render target (rendered by an earlier
 * order < 0 pass) back into the window, either full-screen or into a
 * sub-viewport picture-in-picture rectangle (see setViewport()).
 */
class V_GRAPHICS_API ScreenPass : public RenderPass {
    V_OBJECT_META_DECL;

  public:
    /** @brief Constructs a screen pass.
     *
     * Clearing is disabled by default: the pass draws its textured triangle on
     * top of previously rendered content (e.g. the main scene).
     */
    ScreenPass();

    ~ScreenPass() override;

  public:
    /** @brief Gets the resolved source target this pass samples. */
    raw_ptr<RenderTarget> sourceTarget() const;

    /** @brief Receives the engine-resolved input textures.
     *
     * Stores the first resolved non-null target (matching the single input
     * name this pass declares) as the source to sample.
     *
     * @param inputs Resolved input targets in inputNames() order (borrowed).
     */
    void resolveInputTextures(const std::vector<raw_ptr<RenderTarget>>& inputs) override;

    /** @brief Executes the screen pass.
     *
     * Binds the output target / sub-viewport / clear state like a regular
     * pass, then asks the backend to draw a full-screen triangle sampling the
     * source texture. The scene is ignored.
     *
     * @param scene   Ignored (a screen pass has no scene content).
     * @param backend Backend to render with.
     */
    void execute(raw_ptr<Scene> scene, raw_ptr<RenderBackend> backend) override;

  private:
    /// Source texture sampled by this pass (borrowed; the engine registry keeps it alive).
    raw_ptr<RenderTarget> source_ = nullptr;
};

using ScreenPassPtr = intrusive_ptr<ScreenPass>;

V_GRAPHICS_NS_END
