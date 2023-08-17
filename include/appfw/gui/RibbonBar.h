#pragma once
#include "gui_global.h"
#include "core/Inherit.h"

#include "UIElement.h"

VINE_APPFWGUI_NS_BEGIN

class RibbonTab;
class VINE_APPFWGUI_API RibbonBar : public Inherit<UIElement, RibbonBar>
{

public:
    RibbonBar();

public:
    ULong numTabs() const;
    RibbonTab* tabAt(ULong idx) const;
    void addTab(RibbonTab* tab);
    void removeTab(RibbonTab* tab);
private:
    struct Data;
    Data *const d;
};

VINE_APPFWGUI_NS_END
