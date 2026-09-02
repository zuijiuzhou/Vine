#include <vine/graphics/RenderTarget.hpp>

V_GRAPHICS_NS_BEGIN

V_OBJECT_META_IMPL(RenderTarget, vine::Object);

RenderTarget::RenderTarget() = default;

void RenderTarget::attachColor(ColorFormat format)
{
    color_format_ = format;
    has_color_ = true;
}

void RenderTarget::attachDepth(DepthFormat format)
{
    depth_format_ = format;
    has_depth_ = true;
}

int RenderTarget::width() const
{
    return width_;
}

int RenderTarget::height() const
{
    return height_;
}

void RenderTarget::setSize(int w, int h)
{
    width_ = w;
    height_ = h;
}

std::vector<std::uint8_t> RenderTarget::readColorBuffer() const
{
    // Backend-specific implementation fills this buffer. Without a bound
    // backend this returns an all-zero buffer of the expected size.
    return std::vector<std::uint8_t>(static_cast<std::size_t>(width_) * height_ * 4, 0);
}

std::vector<float> RenderTarget::readDepthBuffer() const
{
    return std::vector<float>(static_cast<std::size_t>(width_) * height_, 0.0f);
}

V_GRAPHICS_NS_END
