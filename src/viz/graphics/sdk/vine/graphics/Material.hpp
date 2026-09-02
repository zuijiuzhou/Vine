#pragma once
#include "graphics_global.hpp"

#include <vine/intrusive_ptr.hpp>
#include <vine/Object.hpp>
#include <vine/RefCounted.hpp>
#include <vine/Colorf.hpp>
#include <vine/String.hpp>

V_GRAPHICS_NS_BEGIN

/**
 * @brief Material class defining rendering properties.
 *
 * Describes the visual appearance of a drawable: diffuse/specular/ambient
 * colors, shininess, opacity, and an optional texture.
 */
class V_GRAPHICS_API Material : public Object, public RefCounted<Material> {
    V_OBJECT_META_DECL;

  public:
    Material();

  public:
    /** @brief Gets the material name. */
    String name() const;

    /** @brief Sets the material name. */
    void setName(const String& name);

    /** @brief Gets the diffuse color (RGBA). */
    Colorf diffuse() const;

    /** @brief Sets the diffuse color.
     *
     * @param color RGBA color in [0, 1].
     */
    void setDiffuse(const Colorf& color);

    /** @brief Gets the specular color (RGB, A is intensity). */
    Colorf specular() const;

    /** @brief Sets the specular color.
     *
     * @param color RGBA color in [0, 1].
     */
    void setSpecular(const Colorf& color);

    /** @brief Gets the ambient color (RGB). */
    Colorf ambient() const;

    /** @brief Sets the ambient color.
     *
     * @param color RGBA color in [0, 1].
     */
    void setAmbient(const Colorf& color);

    /** @brief Gets the shininess (Phong exponent). */
    float shininess() const;

    /** @brief Sets the shininess (Phong exponent).
     *
     * @param shine Phong exponent, typically [1, 128].
     */
    void setShininess(float shine);

    /** @brief Gets the opacity in [0, 1] (1 = fully opaque). */
    float opacity() const;

    /** @brief Sets the opacity.
     *
     * @param alpha Opacity in [0, 1].
     */
    void setOpacity(float alpha);

    /** @brief Gets the texture file path (may be empty). */
    String textureFile() const;

    /** @brief Sets the texture file path.
     *
     * @param path Texture file path, or empty to clear.
     */
    void setTextureFile(const String& path);

  private:
    String name_;
    Colorf diffuse_{ 0.8f, 0.8f, 0.8f, 1.0f };
    Colorf specular_{ 1.0f, 1.0f, 1.0f, 0.5f };
    Colorf ambient_{ 0.2f, 0.2f, 0.2f, 1.0f };
    float shininess_ = 32.0f;
    float opacity_ = 1.0f;
    String texture_file_;
};

using MaterialPtr = intrusive_ptr<Material>;

V_GRAPHICS_NS_END
