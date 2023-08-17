#include <appfw/gui/RibbonBar.h>

#include <vector>

#include <appfw/gui/RibbonTab.h>

VINE_APPFWGUI_NS_BEGIN

struct RibbonBar::Data
{
    std::vector<RibbonTab *> tabs;
};

RibbonBar::RibbonBar()
    : d(new Data())
{
}

ULong RibbonBar::numTabs() const
{
    return d->tabs.size();
}
RibbonTab *RibbonBar::tabAt(ULong idx) const
{
    return d->tabs.at(idx);
}
void RibbonBar::addTab(RibbonTab *tab)
{
}
void RibbonBar::removeTab(RibbonTab *tab)
{
}

VINE_APPFWGUI_NS_END