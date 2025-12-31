#pragma once
#include <vine/RefObject.hpp>
#include "graphics_global.hpp"

VI_GRAPHICS_NS_BEGIN

class VI_GRAPHICS_API Drawable : public RefObject {
  VI_OBJECT_META;
  VI_DISABLE_COPY_MOVE(Drawable);

public:
  Drawable();

public:
  VI_OBJECT_DATA;
};
using DrawablePtr = RefPtr<Drawable>;

VI_GRAPHICS_NS_END