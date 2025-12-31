#pragma once

#include "UIElement.hpp"

VI_APPFWGUI_NS_BEGIN

class VI_APPFW_API RibbonGallery : public UIElement {
    VI_OBJECT_META
    friend class RibbonBar;

  public:
    RibbonGallery();
    virtual ~RibbonGallery();

  private:
    VI_OBJECT_DATA
};

VI_APPFWGUI_NS_END