#include <vine/graphics/RenderTarget.hpp>

V_GRAPHICS_NS_BEGIN

V_OBJECT_META_IMPL(RenderTarget, vine::Object);

RenderTarget::RenderTarget() = default;

void RenderTarget::attachColor(ColorFormat format)
{
    color_formats_.push_back(format);
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

bool RenderTarget::hasColor() const
{
    return !color_formats_.empty();
}

bool RenderTarget::hasDepth() const
{
    return has_depth_;
}

RenderTarget::ColorFormat RenderTarget::colorFormat() const
{
    return color_formats_.empty() ? ColorFormat::RGBA8 : color_formats_.front();
}

int RenderTarget::colorCount() const
{
    return static_cast<int>(color_formats_.size());
}

RenderTarget::ColorFormat RenderTarget::colorFormat(int index) const
{
    if (index < 0 || index >= static_cast<int>(color_formats_.size())) {
        return ColorFormat::RGBA8;
    }
    return color_formats_[static_cast<std::size_t>(index)];
}

RenderTarget::DepthFormat RenderTarget::depthFormat() const
{
    return depth_format_;
}

bool RenderTarget::valid() const
{
    return (!color_formats_.empty() || has_depth_) && width_ > 0 && height_ > 0;
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
