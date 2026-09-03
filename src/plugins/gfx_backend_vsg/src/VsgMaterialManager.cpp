#include <vine/vsg/VsgMaterialManager.hpp>

#include <vine/graphics/Material.hpp>

#include <vsg/state/material.h>

V_VSG_NS_BEGIN

namespace
{

/**
 * @brief Builds a PhongMaterialValue from a Vine material.
 *
 * Maps the Vine material's diffuse/specular/ambient/shininess/opacity onto
 * VSG's PhongMaterial struct. A default grey Phong material is produced when
 * no Vine material is bound.
 *
 * @param material Vine material (may be null).
 * @return VSG Phong material value.
 */
::vsg::ref_ptr<::vsg::PhongMaterialValue> makePhongMaterial(vine::graphics::Material* material)
{
    auto phong = ::vsg::PhongMaterialValue::create();
    auto& m = phong->value();
    m.ambient = ::vsg::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    m.diffuse = ::vsg::vec4(0.8f, 0.8f, 0.8f, 1.0f);
    m.specular = ::vsg::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    m.shininess = 32.0f;
    if (material != nullptr) {
        const auto diffuse = material->diffuse();
        m.diffuse = ::vsg::vec4(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
        const auto specular = material->specular();
        m.specular = ::vsg::vec4(specular.r, specular.g, specular.b, specular.a);
        const auto ambient = material->ambient();
        m.ambient = ::vsg::vec4(ambient.r, ambient.g, ambient.b, ambient.a);
        m.shininess = material->shininess();
        // Opacity rides on the diffuse alpha; the renderer enables blending.
        m.diffuse.a = material->opacity();
    }
    return phong;
}

}  // namespace

struct VsgMaterialManager::Data {
    std::map<vine::graphics::Material*, ::vsg::ref_ptr<::vsg::PhongMaterialValue>> cache;
};

VsgMaterialManager::VsgMaterialManager()
  : d(new Data())
{
}

VsgMaterialManager::~VsgMaterialManager()
{
    delete d;
}

::vsg::ref_ptr<::vsg::PhongMaterialValue> VsgMaterialManager::getOrCreate(
    vine::raw_ptr<vine::graphics::Material> material)
{
    auto it = d->cache.find(material);
    if (it != d->cache.end()) {
        return it->second;
    }
    auto phong = makePhongMaterial(material);
    d->cache[material] = phong;
    return phong;
}

void VsgMaterialManager::updateMaterial(vine::raw_ptr<vine::graphics::Material> material)
{
    if (material == nullptr) {
        return;
    }
    d->cache[material] = makePhongMaterial(material);
}

void VsgMaterialManager::releaseMaterial(vine::raw_ptr<vine::graphics::Material> material)
{
    d->cache.erase(material);
}

void VsgMaterialManager::clear()
{
    d->cache.clear();
}

V_VSG_NS_END
