#pragma once

#include "Widget.hpp"

V_APPFWGUI_NS_BEGIN

class RibbonTab;
class RibbonDropDownItem;
class MainWindow;

class V_APPFW_API RibbonBar : public Widget {
    V_OBJECT_META_DECL

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
    struct Data;
    Data* const d;
};

V_APPFWGUI_NS_END
