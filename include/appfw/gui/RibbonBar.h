#pragma once
#include "gui_global.h"

#include "Control.h"

VI_APPFWGUI_NS_BEGIN

class RibbonTab;
class RibbonDropDownItem;
class MainWindow;
class VI_APPFWGUI_API RibbonBar : public Control
{
    VI_OBJECT_META

    friend class MainWindow;

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

    void appendApplicationMenu(RibbonDropDownItem *mi);

private:
    VI_OBJECT_DATA
};

VI_APPFWGUI_NS_END
