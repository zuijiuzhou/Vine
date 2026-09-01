#include <vine/graphics/RenderTarget.hpp>

V_GRAPHICS_NS_BEGIN

V_OBJECT_META_IMPL(RenderTarget, vine::Object);

struct RenderTarget::Data {
    ColorFormat color_format = ColorFormat::RGBA8;
    DepthFormat depth_format = DepthFormat::D24;
    bool has_color = false;
    bool has_depth = false;
    int width = 1;
    int height = 1;
};

RenderTarget::RenderTarget()
  : d(new Data())
{}

void RenderTarget::attachColor(ColorFormat format)
{
    d->color_format = format;
    d->has_color = true;
}

void RenderTarget::attachDepth(DepthFormat format)
{
    d->depth_format = format;
    d->has_depth = true;
}

int RenderTarget::width() const
{
    return d->width;
}

int RenderTarget::height() const
{
    return d->height;
}

void RenderTarget::setSize(int w, int h)
{
    d->width = w;
    d->height = h;
}

std::vector<std::uint8_t> RenderTarget::readColorBuffer() const
{
    // Backend-specific implementation fills this buffer. Without a bound
    // backend this returns an all-zero buffer of the expected size.
    return std::vector<std::uint8_t>(static_cast<std::size_t>(d->width) * d->height * 4, 0);
}

std::vector<float> RenderTarget::readDepthBuffer() const
{
    return std::vector<float>(static_cast<std::size_t>(d->width) * d->height, 0.0f);
}

V_GRAPHICS_NS_END
