#pragma once
#include "graphics_global.hpp"

#include <vine/RefObject.hpp>

V_GRAPHICS_NS_BEGIN

class V_GRAPHICS_API Scene : public RefObject {
    V_OBJECT_META_DECL;
    V_DISABLE_COPY_MOVE(Scene);

  public:
    Scene();

  private:
    struct Data;
    Data* const d;
    ;
};

V_GRAPHICS_NS_END
