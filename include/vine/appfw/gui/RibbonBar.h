#pragma once
#include "gui_global.h"

#include "Control.h"

VI_APPFWGUI_NS_BEGIN

class RibbonTab;
class RibbonDropDownItem;
class MainWindow;
class VI_APPFWGUI_API RibbonBar : public Control {
    VI_OBJECT_META

    friend class MainWindow;

  public:
    RibbonBar(MainWindow* wnd);
    virtual ~RibbonBar();

  public:
    int        numTabs() const;
    RibbonTab* tabAt(int idx) const;
    void       addTab(RibbonTab* tab);
    void       removeTab(RibbonTab* tab);
    int        currentIndex();
    void       currentIndex(int idx);

    void appendApplicationMenu(RibbonDropDownItem* mi);

  private:
    VI_OBJECT_DATA
};

VI_APPFWGUI_NS_END
