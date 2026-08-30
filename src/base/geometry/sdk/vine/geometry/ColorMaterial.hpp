#pragma once

#include "geometry_global.hpp"

#include <vine/Colorf.hpp>

#include "Material.hpp"

V_GEOMETRY_NS_BEGIN

/**
 * @brief A simple material carrying a single base color.
 */
class V_GEOMETRY_API ColorMaterial : public Material {
    V_OBJECT_META_DECL;

  public:
    /**
     * @brief Constructs a white material.
     */
    ColorMaterial();

    /**
     * @brief Constructs a material from a color.
     *
     * @param color Base color (linear RGBA, 0..1).
     */
    explicit ColorMaterial(const vine::Colorf& color);

  public:
    /**
     * @brief Returns the base color.
     *
     * @return The material color.
     */
    const vine::Colorf& color() const;

    /**
     * @brief Sets the base color.
     *
     * @param color New color.
     */
    void setColor(const vine::Colorf& color);

    [[nodiscard]]
    const char* typeName() const override;

  private:
    /// Base color.
    vine::Colorf color_{ 1.0f, 1.0f, 1.0f, 1.0f };
};

V_GEOMETRY_NS_END
