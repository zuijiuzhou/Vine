#pragma once

#include "UIElement.hpp"

V_APPFWGUI_NS_BEGIN

class V_APPFW_API RibbonButton : public UIElement {
    V_OBJECT_META_DECL

  public:
    RibbonButton();
    virtual ~RibbonButton();

    void text(const String& t);
    String text() const;

    void setData(void* dptr);
    void* data() const;

  private:
    struct Data;
    Data* const d;
};

V_APPFWGUI_NS_END
