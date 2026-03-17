#pragma once

#include "UIElement.hpp"

V_APPFWGUI_NS_BEGIN

class V_APPFW_API RibbonGallery : public UIElement {
    V_OBJECT_META_DECL
    friend class RibbonBar;

  public:
    RibbonGallery();
    virtual ~RibbonGallery();

  private:
    struct Data;
    Data* const d;
};

V_APPFWGUI_NS_END
