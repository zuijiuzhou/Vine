#pragma once

#include "geometry_global.hpp"

#include <vine/RefObject.hpp>

V_GEOMETRY_NS_BEGIN

class V_GEOMETRY_API Shape : public vine::RefObject {
    V_OBJECT_META_DECL;

  public:
    Shape();

  public:
};

V_GEOMETRY_NS_END