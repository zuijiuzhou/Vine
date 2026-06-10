#pragma once

#include "UIElement.hpp"

V_APPFWGUI_NS_BEGIN

class RibbonButton;

class V_APPFW_API RibbonGroup : public UIElement {
    V_OBJECT_META_DECL

  public:
    RibbonGroup();
    virtual ~RibbonGroup();

    void title(const String& t);
    String title() const;

    void addButton(RibbonButton* b);
    void removeButton(RibbonButton* b);

  private:
    struct Data;
    Data* const d;
};

V_APPFWGUI_NS_END
