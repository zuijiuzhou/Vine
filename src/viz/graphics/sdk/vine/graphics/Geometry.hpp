#pragma once
#include "Drawable.hpp"
#include "graphics_global.hpp"

V_GRAPHICS_NS_BEGIN

class V_GRAPHICS_API Geometry : public Drawable {
    V_OBJECT_META_DECL;

  public:
    Geometry();

  public:
    struct Data;
    Data* const d;
    ;
};

using GeometryPtr = RefPtr<Geometry>;

V_GRAPHICS_NS_END
