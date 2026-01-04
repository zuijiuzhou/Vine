#pragma once

#include "Widget.hpp"

VI_APPFWGUI_NS_BEGIN

class RibbonBar;
class RibbonGroup;

class VI_APPFW_API RibbonTab : public Widget {
    VI_OBJECT_META

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
    VI_OBJECT_DATA
};

VI_APPFWGUI_NS_END
