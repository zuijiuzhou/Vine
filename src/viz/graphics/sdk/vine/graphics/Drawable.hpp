#pragma once
#include "graphics_global.hpp"
#include <vine/RefObject.hpp>

V_GRAPHICS_NS_BEGIN

class V_GRAPHICS_API Drawable : public RefObject {
    V_OBJECT_META_DECL;

  public:
    Drawable();

  public:
    struct Data;
    Data* const d;
    ;
};

using DrawablePtr = RefPtr<Drawable>;

V_GRAPHICS_NS_END
