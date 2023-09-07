#include <appfw/gui/RibbonBar.h>

#include <vector>
#include <SARibbon.h>
#include <core/Exception.h>
#include <appfw/gui/RibbonTab.h>
#include <appfw/gui/MainWindow.h>

VI_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(RibbonBar, UIElement)

struct RibbonBar::Data
{
    std::vector<RibbonTab *> tabs;
    RibbonTab *current_tab = nullptr;
    MainWindow* wnd;
};

namespace{
    using itype = SARibbonBar;
}

RibbonBar::RibbonBar(MainWindow* wnd)
    : Control(static_cast<SARibbonMainWindow*>(wnd->impl())->ribbonBar())
    , d(new Data())
{
    d->wnd = wnd;
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
    VI_CHECK_NULL(tab)
    auto w = impl<itype>();
    w->addCategoryPage(tab->impl<SARibbonCategory>());
    return this;
}

RibbonBar *RibbonBar::removeTab(RibbonTab *tab)
{
    VI_CHECK_NULL(tab)
    auto w = impl<itype>();
    w->removeCategory(tab->impl<SARibbonCategory>());
    return this;
}

Int RibbonBar::currentIndex()
{
    auto w = impl<itype>();
    auto idx = w->currentIndex();
    return idx;
}

RibbonBar *RibbonBar::currentIndex(Int idx)
{
    auto w = impl<itype>();
    w->setCurrentIndex(idx);
    return this;
}

VI_APPFWGUI_NS_END