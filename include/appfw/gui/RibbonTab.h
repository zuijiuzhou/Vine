#pragma once
#include "gui_global.h"

#include "Control.h"

VI_APPFWGUI_NS_BEGIN

class RibbonBar;
class RibbonGroup;

class VI_APPFWGUI_API RibbonTab : public Control
{
    VI_OBJECT_META

    friend class RibbonBar;

public:
    RibbonTab();
    virtual ~RibbonTab();

public:
    String title() const;
    RibbonTab *title(const String &ti);

    RibbonTab *addGroup(RibbonGroup *group);
    RibbonTab *removeGroup(RibbonGroup *group);
    Int32 numGroups() const;
    RibbonGroup *groupAt(Int32 idx) const;

private:
    VI_OBJECT_DATA
};

VI_APPFWGUI_NS_END
