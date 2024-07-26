#pragma once
#include "graphics_global.h"
#include "Drawable.h"

VI_GRAPHICS_NS_BEGIN

class VI_GRAPHICS_API Geometry : public Drawable {
  VI_OBJECT_META;

public:
  Geometry();

public:
  VI_OBJECT_DATA;
};
using GeometryPtr = RefPtr<Geometry>;

VI_GRAPHICS_NS_END