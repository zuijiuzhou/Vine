#pragma once
#include "gui_global.h"

#include "Control.h"

VINE_APPFWGUI_NS_BEGIN

class RibbonTab;
class VINE_APPFWGUI_API RibbonBar : Control
{
    VI_OBJECT_META

friend class MainWindow;

private:
    VI_DISABLE_COPY_MOVE(RibbonBar)

public:
    RibbonBar(MainWindow* wnd);

public:
    ULong numTabs() const;
    RibbonTab *tabAt(ULong idx) const;
    RibbonBar *addTab(RibbonTab *tab);
    RibbonBar *removeTab(RibbonTab *tab);
    RibbonTab *currentTab();
    RibbonBar *currentTab(ULong idx);
    RibbonBar *currentTab(const String &name);

private:
    struct Data;
    Data *const d;
};

VINE_APPFWGUI_NS_END
