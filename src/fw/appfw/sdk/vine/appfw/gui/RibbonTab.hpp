#pragma once

#include "Widget.hpp"

V_APPFWGUI_NS_BEGIN

class RibbonBar;
class RibbonGroup;

class V_APPFW_API RibbonTab : public Widget {
    V_OBJECT_META_DECL

    friend class RibbonBar;

  public:
    RibbonTab();
    virtual ~RibbonTab();

  public:
    String title() const;
    void   title(const String& ti);

    void addGroup(RibbonGroup* group);
    void removeGroup(RibbonGroup* group);
    int  numGroups() const;
    void groupAt(int idx) const;

  private:
    struct Data;
    Data* const d;
};

V_APPFWGUI_NS_END
