#pragma once

#include "UIElement.hpp"

V_APPFWGUI_NS_BEGIN

class V_APPFW_API RibbonDropDownItem : public UIElement {
    V_OBJECT_META_DECL
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
    struct Data;
    Data* const d;
};

V_APPFWGUI_NS_END
