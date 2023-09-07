#pragma once
#include "gui_global.h"

#include "Control.h"

VI_APPFWGUI_NS_BEGIN

class RibbonTab;
class VI_APPFWGUI_API RibbonBar : Control
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
    Int currentIndex();
    RibbonBar *currentIndex(Int idx);

private:
    struct Data;
    Data *const d;
};

VI_APPFWGUI_NS_END
