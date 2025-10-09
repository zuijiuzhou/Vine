#pragma once
#include "graphics_global.hpp"

#include <vine/core/Object.hpp>

VI_GRAPHICS_NS_BEGIN

class VI_GRAPHICS_API Scene : public Object {
  VI_OBJECT_META;
  VI_DISABLE_COPY_MOVE(Scene);

public:
  Scene();

private:
  VI_OBJECT_DATA;
};

VI_GRAPHICS_NS_END