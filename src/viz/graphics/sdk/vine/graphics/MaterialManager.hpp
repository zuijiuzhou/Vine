#pragma once
#include "graphics_global.hpp"

#include <vine/intrusive_ptr.hpp>
#include <vine/Object.hpp>
#include <vine/RefCounted.hpp>
#include <vine/raw_ptr.hpp>

V_GRAPHICS_NS_BEGIN

class Material;

/**
 * @brief Abstract material manager converting user materials to backend resources.
 *
 * MaterialManager defines the lifecycle contract for translating a
 * platform-independent vine::graphics::Material (pure attributes) into
 * backend-specific rendering resources (e.g. a VSG PhongMaterialValue).
 * Concrete backends implement this interface and own their resource cache:
 * multiple drawables sharing one Material must reuse a single backend
 * resource.
 *
 * The interface deliberately exposes no backend types; resource translation
 * and caching happen inside the concrete implementation.
 */
class V_GRAPHICS_API MaterialManager : public Object, public RefCounted<MaterialManager> {
    V_OBJECT_META_DECL;

  public:
    ~MaterialManager() override = default;

    /** @brief Updates (or creates) the backend resource for a material.
     *
     * Called when the material's attributes change so the backend can refresh
     * its cached resource.
     *
     * @param material Material whose backend resource must be (re)built.
     */
    virtual void updateMaterial(raw_ptr<Material> material) = 0;

    /** @brief Releases the backend resource for a material.
     *
     * @param material Material whose backend resource is released.
     */
    virtual void releaseMaterial(raw_ptr<Material> material) = 0;

    /** @brief Releases all cached backend resources. */
    virtual void clear() = 0;

  protected:
    MaterialManager() = default;
};

V_GRAPHICS_NS_END
