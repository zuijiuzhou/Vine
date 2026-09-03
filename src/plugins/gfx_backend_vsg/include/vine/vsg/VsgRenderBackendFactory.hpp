#pragma once
#include "vsg_global.hpp"

#include <vine/graphics/RenderBackendRegistry.hpp>
#include <vine/raw_ptr.hpp>

V_VSG_NS_BEGIN

/**
 * @brief Factory creating a VsgRenderer, self-registered into the render
 * backend registry.
 *
 * Lets the application create the VSG backend by name ("vsg") through
 * vine::graphics::RenderBackendRegistry without a compile-time dependency on
 * this module or on VulkanSceneGraph.
 */
class V_VSG_API VsgRenderBackendFactory : public vine::graphics::RenderBackendFactory {
  public:
    VsgRenderBackendFactory();
    ~VsgRenderBackendFactory() override;

  public:
    /** @brief Gets the backend's static metadata ("vsg").
     *
     * @return The VSG backend metadata.
     */
    vine::graphics::RenderBackendInfo info() const override;

    /** @brief Creates a VsgRenderer.
     *
     * @param scene  Scene bound to the backend.
     * @param camera Camera used by the backend.
     * @return New VsgRenderer; the caller owns it.
     */
    vine::intrusive_ptr<vine::graphics::RenderBackend> create(
        vine::raw_ptr<vine::graphics::Scene> scene,
        vine::raw_ptr<vine::graphics::Camera> camera) override;
};

V_VSG_NS_END
