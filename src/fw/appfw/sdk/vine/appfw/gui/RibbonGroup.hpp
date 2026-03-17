#pragma once

#include "Widget.hpp"

V_APPFWGUI_NS_BEGIN

class RibbonTab;

class V_APPFW_API RibbonGroup : public Widget {
    V_OBJECT_META_DECL

    friend class RibbonTab;

  public:
    RibbonGroup();
    virtual ~RibbonGroup();

  public:
    String title() const;
    void   title(const String& ti);

  private:
    struct Data;
    Data* const d;
};

V_APPFWGUI_NS_END
