#include <appfw/gui/RibbonBar.h>

#include <vector>

#include <core/Exception.h>
#include <appfw/gui/RibbonTab.h>

VINE_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(RibbonBar, UIElement)

struct RibbonBar::Data
{
    std::vector<RibbonTab *> tabs;
    RibbonTab *current_tab = nullptr;
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

RibbonBar *RibbonBar::addTab(RibbonTab *tab)
{
    VI_THROW_IF_NULL(tab)
    return this;
}

RibbonBar *RibbonBar::removeTab(RibbonTab *tab)
{
    VI_THROW_IF_NULL(tab)
    return this;
}

RibbonTab *RibbonBar::currentTab()
{
    return d->current_tab;
}

RibbonBar *RibbonBar::currentTab(ULong idx)
{
    return this;
}

RibbonBar *RibbonBar::currentTab(const String &name)
{
    return this;
}

VINE_APPFWGUI_NS_END