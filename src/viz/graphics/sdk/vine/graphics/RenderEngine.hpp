#pragma once
#include "graphics_global.hpp"

#include <vine/intrusive_ptr.hpp>
#include <vine/Object.hpp>
#include <vine/RefCounted.hpp>

V_GRAPHICS_NS_BEGIN

class Camera;
class Scene;
class RenderPass;
class RenderBackend;

/**
 * @brief High-level render engine managing the frame loop and render state.
 *
 * RenderEngine owns the default scene, camera, and main render pass, and
 * drives one frame per call to frame(): begin, execute the main pass, end,
 * and swap buffers. It is platform-independent and delegates actual drawing
 * to a RenderBackend supplied by the caller.
 */
class V_GRAPHICS_API RenderEngine : public Object, public RefCounted<RenderEngine> {
    V_OBJECT_META_DECL;

  public:
    /** @brief Constructs an engine bound to a backend.
     *
     * The backend must outlive the engine. A default scene, camera and main
     * render pass are created automatically.
     *
     * @param backend Backend used for drawing.
     */
    explicit RenderEngine(RenderBackend* backend);
    ~RenderEngine();

  public:
    /** @brief Gets the bound render backend. */
    RenderBackend* backend() const;

    /** @brief Initializes the backend.
     *
     * @return true when the backend initialized successfully.
     */
    bool initialize();

    /** @brief Releases backend resources. */
    void shutdown();

    /** @brief Renders one frame (begin, main pass, end, swap). */
    void frame();

    /** @brief Sets the scene to render. */
    void setScene(Scene* scene);

    /** @brief Gets the scene to render. */
    Scene* scene() const;

    /** @brief Sets the camera used by the main pass. */
    void setCamera(Camera* camera);

    /** @brief Gets the camera used by the main pass. */
    Camera* camera() const;

    /** @brief Sets the main render pass. */
    void setMainPass(RenderPass* pass);

    /** @brief Gets the main render pass. */
    RenderPass* mainPass() const;

  private:
    struct Data;
    Data* const d;
};

using RenderEnginePtr = intrusive_ptr<RenderEngine>;

V_GRAPHICS_NS_END
