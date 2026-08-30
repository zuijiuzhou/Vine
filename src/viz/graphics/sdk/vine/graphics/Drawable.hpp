#pragma once
#include "graphics_global.hpp"

#include <vine/IntrusivePtr.hpp>
#include <vine/Object.hpp>
#include <vine/RefCounted.hpp>

V_GRAPHICS_NS_BEGIN

class V_GRAPHICS_API Drawable : public Object, public RefCounted<Drawable> {
    V_OBJECT_META_DECL;

  public:
    Drawable();

  public:
    struct Data;
    Data* const d;
    ;
};

using DrawablePtr = IntrusivePtr<Drawable>;

V_GRAPHICS_NS_END
