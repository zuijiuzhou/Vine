#pragma once
#include "vsg_global.hpp"

#include <vine/graphics/MaterialManager.hpp>

#include <vsg/core/ref_ptr.h>
#include <vsg/state/material.h>

#include <map>

namespace vine::graphics
{
class Material;
}

V_VSG_NS_BEGIN

/**
 * @brief VSG material manager: converts vine materials to Phong resources.
 *
 * Implements vine::graphics::MaterialManager by translating a
 * vine::graphics::Material (pure attributes) into a cached
 * vsg::PhongMaterialValue. Multiple drawables sharing the same Material
 * reuse a single Phong resource, avoiding redundant GPU data and pipeline
 * variants.
 */
class V_VSG_API VsgMaterialManager : public vine::graphics::MaterialManager {
  public:
    VsgMaterialManager();
    ~VsgMaterialManager() override;

  public:
    /** @brief Gets (or creates) the Phong resource for a material.
     *
     * @param material Vine material (may be null → default grey).
     * @return Cached Phong material value.
     */
    ::vsg::ref_ptr<::vsg::PhongMaterialValue> getOrCreate(vine::graphics::Material* material);

  public:
    /** @brief Rebuilds the cached resource for a material. */
    void updateMaterial(vine::graphics::Material* material) override;

    /** @brief Releases the cached resource for a material. */
    void releaseMaterial(vine::graphics::Material* material) override;

    /** @brief Releases all cached resources. */
    void clear() override;

  private:
    struct Data;
    Data* const d;
};

V_VSG_NS_END
