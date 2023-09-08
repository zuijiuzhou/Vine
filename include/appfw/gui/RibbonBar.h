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
    RibbonBar(MainWindow *wnd);
    virtual ~RibbonBar();

public:
    Int32 numTabs() const;
    RibbonTab *tabAt(Int32 idx) const;
    RibbonBar *addTab(RibbonTab *tab);
    RibbonBar *removeTab(RibbonTab *tab);
    Int32 currentIndex();
    RibbonBar *currentIndex(Int32 idx);

private:
    VI_OBJECT_DATA
};

VI_APPFWGUI_NS_END
