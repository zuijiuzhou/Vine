#include <appfw/gui/RibbonBar.h>

#include <SARibbon.h>
#include <core/Exception.h>
#include <appfw/gui/RibbonTab.h>
#include <appfw/gui/MainWindow.h>

VI_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(RibbonBar, Control)

struct RibbonBar::Data
{
    std::vector<RibbonTab *> tabs;
    MainWindow *wnd;
};

namespace
{
    using itype = SARibbonBar;
}

RibbonBar::RibbonBar(MainWindow *wnd)
    : Control(static_cast<SARibbonMainWindow *>(wnd->impl())->ribbonBar()), d(new Data())
{
    d->wnd = wnd;
}

RibbonBar::~RibbonBar(){
    delete d;
}

Int32 RibbonBar::numTabs() const
{
    return d->tabs.size();
}

RibbonTab *RibbonBar::tabAt(Int32 idx) const
{
    return d->tabs.at(idx);
}

RibbonBar *RibbonBar::addTab(RibbonTab *tab)
{
    VI_CHECK_NULL(tab)
    if (std::any_of(d->tabs.begin(), d->tabs.end(), [tab](RibbonTab *t)
                    { return tab == t; }))
        return this;
    auto w = impl<itype>();
    w->addCategoryPage(tab->impl<SARibbonCategory>());
    return this;
}

RibbonBar *RibbonBar::removeTab(RibbonTab *tab)
{
    VI_CHECK_NULL(tab)
    if (std::none_of(d->tabs.begin(), d->tabs.end(), [tab](RibbonTab *t)
                     { return tab == t; }))
        return this;
    auto w = impl<itype>();
    w->removeCategory(tab->impl<SARibbonCategory>());
    return this;
}

Int32 RibbonBar::currentIndex()
{
    auto w = impl<itype>();
    return w->currentIndex();
}

RibbonBar *RibbonBar::currentIndex(Int32 idx)
{
    auto w = impl<itype>();
    w->setCurrentIndex(idx);
    return this;
}

VI_APPFWGUI_NS_END