#pragma once
#include "gui_global.h"

#include "UIElement.h"

VINE_APPFWGUI_NS_BEGIN

class RibbonTab;
class VINE_APPFWGUI_API RibbonBar : UIElement
{
    VI_OBJECT_META

private:
    VI_DISABLE_COPY_MOVE(RibbonBar)

public:
    RibbonBar();

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
