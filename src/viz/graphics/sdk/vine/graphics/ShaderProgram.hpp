#pragma once
#include "graphics_global.hpp"

#include <vine/intrusive_ptr.hpp>
#include <vine/Object.hpp>
#include <vine/RefCounted.hpp>
#include <vine/String.hpp>

#include <cstddef>
#include <vector>

V_GRAPHICS_NS_BEGIN

/**
 * @brief Kind of a shader stage.
 */
enum class ShaderStageType
{
    Vertex,   ///< Vertex shader.
    Fragment, ///< Fragment (pixel) shader.
    Compute,  ///< Compute shader (reserved).
};

/**
 * @brief One shader stage: a GLSL source with an entry point.
 *
 * A value type aggregating the authored source of a single stage. The SDK is
 * backend-agnostic and never compiles; the render backend compiles the GLSL
 * to SPIR-V at run time (vsg ShaderCompiler, glslang) or consumes
 * pre-compiled SPIR-V supplied through the program.
 */
struct V_GRAPHICS_API ShaderStage
{
    ShaderStageType type = ShaderStageType::Vertex;  ///< Stage kind.
    String source;                                   ///< GLSL source for the stage.
    String entryPoint = u8"main";                    ///< Entry function name.
};

/**
 * @brief Backend-agnostic, user-authored shader program (a set of stages).
 *
 * ShaderProgram is the thin programmable-shading handle: it simply groups the
 * GLSL sources of the stages (vertex + fragment today) that replace the
 * engine's default program for the objects it is bound to. Parameters,
 * texture slots and pass-level inputs are layered on later (see
 * .ai/design/graphics-shader.md). A Geometry whose program() is null uses the
 * engine default program (ShaderPreset / material driven) — zero regression.
 */
class V_GRAPHICS_API ShaderProgram : public Object, public RefCounted<ShaderProgram> {
    V_OBJECT_META_DECL;

  public:
    ShaderProgram();
    ~ShaderProgram();

  public:
    /** @brief Gets the program name. */
    String name() const;

    /** @brief Sets the program name. */
    void setName(const String& name);

    /** @brief Appends a shader stage.
     *
     * @param stage Stage to add (its source is kept by value).
     */
    void addStage(const ShaderStage& stage);

    /** @brief Gets the number of stages in the program. */
    std::size_t stageCount() const;

    /** @brief Gets the stage at @p index.
     *
     * @param index Stage index.
     * @return Stage, or null when out of range.
     */
    const ShaderStage* stage(std::size_t index) const;

    /** @brief Gets all stages (read-only). */
    const std::vector<ShaderStage>& stages() const;

  private:
    String name_;
    std::vector<ShaderStage> stages_;
};

using ShaderProgramPtr = intrusive_ptr<ShaderProgram>;

V_GRAPHICS_NS_END
