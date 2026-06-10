#pragma once

#include "UIElement.hpp"

V_APPFWGUI_NS_BEGIN

class RibbonGroup;

class V_APPFW_API RibbonTab : public UIElement {
    V_OBJECT_META_DECL

  public:
    RibbonTab();
    virtual ~RibbonTab();

    void title(const String& t);
    String title() const;

    void addGroup(RibbonGroup* g);
    void removeGroup(RibbonGroup* g);

  private:
    struct Data;
    Data* const d;
};

V_APPFWGUI_NS_END
