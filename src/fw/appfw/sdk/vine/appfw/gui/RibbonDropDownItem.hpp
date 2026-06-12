#pragma once

#include "UIElement.hpp"

V_APPFWGUI_NS_BEGIN

class V_APPFW_API RibbonDropDownItem : public UIElement {
    V_OBJECT_META_DECL

  public:
    RibbonDropDownItem();
    virtual ~RibbonDropDownItem();

    void text(const String& t);
    String text() const;

    void setData(void* dptr);
    void* data() const;

  private:
    struct Data;
    Data*       dptr();
    const Data* dptr() const;
};

V_APPFWGUI_NS_END
