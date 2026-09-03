#pragma once
#include "graphics_global.hpp"

#include <vine/intrusive_ptr.hpp>
#include <vine/Object.hpp>
#include <vine/RefCounted.hpp>
#include <cstdint>
#include <vector>

V_GRAPHICS_NS_BEGIN

/**
 * @brief Render target managing color and depth buffers.
 *
 * Represents an off-screen frame buffer. Supports various color and depth
 * formats and provides buffer readback for post-processing or export.
 */
class V_GRAPHICS_API RenderTarget : public Object, public RefCounted<RenderTarget> {
    V_OBJECT_META_DECL;

  public:
    enum class ColorFormat {
        RGBA8,    ///< 8-bit unsigned normalized RGBA.
        RGBA16F,  ///< 16-bit float RGBA.
        RGBA32F,  ///< 32-bit float RGBA.
    };

    enum class DepthFormat {
        D16,   ///< 16-bit depth.
        D24,   ///< 24-bit depth.
        D32,   ///< 32-bit unsigned depth.
        D32F,  ///< 32-bit float depth.
    };

  public:
    RenderTarget();

  public:
    /** @brief Attaches a color buffer.
     *
     * @param format Color buffer format.
     */
    void attachColor(ColorFormat format);

    /** @brief Attaches a depth buffer.
     *
     * @param format Depth buffer format.
     */
    void attachDepth(DepthFormat format);

    /** @brief Gets the render target width. */
    int width() const;

    /** @brief Gets the render target height. */
    int height() const;

    /** @brief Sets the render target dimensions.
     *
     * @param w Width in pixels.
     * @param h Height in pixels.
     */
    void setSize(int w, int h);

    /** @brief Returns whether a color attachment is configured. */
    bool hasColor() const;

    /** @brief Returns whether a depth attachment is configured. */
    bool hasDepth() const;

    /** @brief Gets the configured color attachment format. */
    ColorFormat colorFormat() const;

    /** @brief Gets the configured depth attachment format. */
    DepthFormat depthFormat() const;

    /** @brief Returns whether this is a usable off-screen buffer.
     *
     * A target is valid when it has at least one attachment and positive
     * dimensions; a valid (non-null) target passed to a backend means
     * "render into this off-screen buffer" as opposed to the default
     * framebuffer.
     *
     * @return true when the target has an attachment and a positive size.
     */
    bool valid() const;

    /** @brief Reads back the color buffer.
     *
     * @return Packed RGBA8 pixel data (width * height * 4 bytes).
     */
    std::vector<std::uint8_t> readColorBuffer() const;

    /** @brief Reads back the depth buffer.
     *
     * @return Depth values in [0, 1] (width * height floats).
     */
    std::vector<float> readDepthBuffer() const;

  private:
    ColorFormat color_format_ = ColorFormat::RGBA8;
    DepthFormat depth_format_ = DepthFormat::D24;
    bool has_color_ = false;
    bool has_depth_ = false;
    int width_ = 1;
    int height_ = 1;
};

using RenderTargetPtr = intrusive_ptr<RenderTarget>;

V_GRAPHICS_NS_END
