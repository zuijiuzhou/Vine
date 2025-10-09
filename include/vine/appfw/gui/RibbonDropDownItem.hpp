#pragma once

#include "gui_global.hpp"

#include "UIElement.hpp"

VI_APPFWGUI_NS_BEGIN

class VI_APPFWGUI_API RibbonDropDownItem : public UIElement {
    VI_OBJECT_META
    friend class RibbonBar;

  public:
    RibbonDropDownItem();
    virtual ~RibbonDropDownItem();

  public:
    String text() const;
    void   text(const String& txt);

    void* data() const;
    void  data(void* v);

  private:
    VI_OBJECT_DATA
};

VI_APPFWGUI_NS_END