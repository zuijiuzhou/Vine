#pragma once
#include <vine/core/Object.h>
#include "graphics_global.h"

VI_GRAPHICS_NS_BEGIN

class VI_GRAPHICS_API Drawable : public Object {
  VI_OBJECT_META;
  VI_DISABLE_COPY_MOVE(Drawable);

public:
  Drawable();

public:
  VI_OBJECT_DATA;
};
using DrawablePtr = RefPtr<Drawable>;

VI_GRAPHICS_NS_END