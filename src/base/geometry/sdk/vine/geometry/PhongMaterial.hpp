#pragma once

#include "geometry_global.hpp"

#include <vine/Colorf.hpp>

#include "Material.hpp"

V_GEOMETRY_NS_BEGIN

/**
 * @brief A Phong shading material with ambient/diffuse/specular components.
 */
class V_GEOMETRY_API PhongMaterial : public Material {
    V_OBJECT_META_DECL;

  public:
    /**
     * @brief Constructs a default gray Phong material.
     */
    PhongMaterial();

    /**
     * @brief Constructs a material from all components.
     *
     * @param ambient   Ambient reflectance (linear RGBA).
     * @param diffuse   Diffuse reflectance (linear RGBA).
     * @param specular  Specular reflectance (linear RGBA).
     * @param shininess Specular exponent; larger values give sharper highlights.
     */
    PhongMaterial(const vine::Colorf& ambient, const vine::Colorf& diffuse,
        const vine::Colorf& specular, float shininess = 32.0f);

  public:
    /**
     * @brief Returns the ambient reflectance.
     *
     * @return Ambient color.
     */
    const vine::Colorf& ambient() const;

    /**
     * @brief Sets the ambient reflectance.
     *
     * @param ambient New ambient color.
     */
    void setAmbient(const vine::Colorf& ambient);

    /**
     * @brief Returns the diffuse reflectance.
     *
     * @return Diffuse color.
     */
    const vine::Colorf& diffuse() const;

    /**
     * @brief Sets the diffuse reflectance.
     *
     * @param diffuse New diffuse color.
     */
    void setDiffuse(const vine::Colorf& diffuse);

    /**
     * @brief Returns the specular reflectance.
     *
     * @return Specular color.
     */
    const vine::Colorf& specular() const;

    /**
     * @brief Sets the specular reflectance.
     *
     * @param specular New specular color.
     */
    void setSpecular(const vine::Colorf& specular);

    /**
     * @brief Returns the specular exponent.
     *
     * @return Shininess value.
     */
    float shininess() const;

    /**
     * @brief Sets the specular exponent.
     *
     * @param shininess New value.
     */
    void setShininess(float shininess);

    [[nodiscard]]
    const char* typeName() const override;

  private:
    /// Ambient reflectance.
    vine::Colorf ambient_{ 0.2f, 0.2f, 0.2f, 1.0f };
    /// Diffuse reflectance.
    vine::Colorf diffuse_{ 0.8f, 0.8f, 0.8f, 1.0f };
    /// Specular reflectance.
    vine::Colorf specular_{ 1.0f, 1.0f, 1.0f, 1.0f };
    /// Specular exponent.
    float shininess_ = 32.0f;
};

V_GEOMETRY_NS_END
